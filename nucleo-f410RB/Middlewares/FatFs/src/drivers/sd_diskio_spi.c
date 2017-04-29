/**
 ******************************************************************************
 * @file    user_diskio.c
 * @brief   This file includes a diskio driver skeleton to be completed by the user.
 ******************************************************************************
 *
 * COPYRIGHT(c) 2016 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "ff_gen_drv.h"
#include "sd_diskio_spi.h"
#ifdef DEBUG
#include <diag/Trace.h>
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* SD commands */
#define CMD0  (0)     /* GO_IDLE_STATE */
#define CMD1  (1)     /* SEND_OP_COND (MMC) */
#define ACMD41  (0x80+41) /* SEND_OP_COND (SDC) */
#define CMD8  (8)     /* SEND_IF_COND */
#define CMD9  (9)     /* SEND_CSD */
#define CMD10 (10)    /* SEND_CID */
#define CMD12 (12)    /* STOP_TRANSMISSION */
#define ACMD13  (0x80+13) /* SD_STATUS (SDC) */
#define CMD16 (16)    /* SET_BLOCKLEN */
#define CMD17 (17)    /* READ_SINGLE_BLOCK */
#define CMD18 (18)    /* READ_MULTIPLE_BLOCK */
#define CMD23 (23)    /* SET_BLOCK_COUNT (MMC) */
#define ACMD23  (0x80+23) /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24 (24)    /* WRITE_BLOCK */
#define CMD25 (25)    /* WRITE_MULTIPLE_BLOCK */
#define CMD32 (32)    /* ERASE_ER_BLK_START */
#define CMD33 (33)    /* ERASE_ER_BLK_END */
#define CMD38 (38)    /* ERASE */
#define CMD55 (55)    /* APP_CMD */
#define CMD58 (58)    /* READ_OCR */

#define CT_MMC    0x01    /* MMC ver 3 */
#define CT_SD1    0x02    /* SD ver 1 */
#define CT_SD2    0x04    /* SD ver 2 */
#define CT_SDC    (CT_SD1|CT_SD2) /* SD */
#define CT_BLOCK  0x08    /* Block addressing */

/* Private variables ---------------------------------------------------------*/
static volatile DSTATUS Stat = STA_NOINIT; /* Physical drive status */
static volatile BYTE    CardType = 0;      /* Card type flags */
static SPI_Interface_Init spi;


/* Private function prototypes -----------------------------------------------*/
static void SPI_Deselect (void);
static int SPI_ReadDatablock (BYTE *buff, UINT btr);
static int SPI_Select (void);
static BYTE SPI_SendCMD(uint8_t cmd, uint32_t arg);
static int SPI_WaitReady (UINT wt);

static DSTATUS SD_SPI_initialize(BYTE pdrv);
static DSTATUS SD_SPI_status(BYTE pdrv);
static DRESULT SD_SPI_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
static DRESULT SD_SPI_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
static DRESULT SD_SPI_ioctl(BYTE pdrv, BYTE cmd, void *buff);
#endif /* _USE_IOCTL == 1 */

Diskio_drvTypeDef SD_SPI_Driver = { SD_SPI_initialize, SD_SPI_status, SD_SPI_read,
#if  _USE_WRITE
    SD_SPI_write,
#endif  /* _USE_WRITE == 1 */  
#if  _USE_IOCTL == 1
    SD_SPI_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/**
 * @brief  Initialize the library with the SPI configuration parameters
 * @param SS_GPIO_Port: GPIO port used to drive the Slave Select signal
 * param  SS_GPIO_Pin: GPIO pin used to drive the Slave Select signal
 * param  hspi: Reference to the SPI_HandleTypeDef handle
 * @retval : None
 */
void SD_SPI_Configure(GPIO_TypeDef *SS_GPIO_Port, uint16_t SS_GPIO_Pin, SPI_HandleTypeDef *hspi) {
  spi.SS_GPIO_Port = SS_GPIO_Port;
  spi.SS_GPIO_Pin = SS_GPIO_Pin;
  spi.hspi = hspi;
}

/**
 * @brief  Deselect the SD
 * @retval : None
 */
static
void SPI_Deselect (void) {
  uint8_t DUMMY_CLK = 0xFF;

  HAL_GPIO_WritePin(spi.SS_GPIO_Port, spi.SS_GPIO_Pin, GPIO_PIN_SET);
  HAL_SPI_Transmit(spi.hspi, &DUMMY_CLK, 1, _FS_TIMEOUT);
}

/**
 * @brief  Receive a datablock (sector) from the SD
 * @param buff: Data buffer
 * param  btr: Data block length (bytes)
 * @retval : 1:OK, 0:Error
 */
static
int SPI_ReadDatablock (BYTE *buff, UINT btr) {
	BYTE token = 0xFF;
	uint32_t tick = HAL_GetTick();

	do {							/* Wait for DataStart token in timeout of 200ms */
		if(HAL_SPI_TransmitReceive(spi.hspi, &token, &token, 1, _FS_TIMEOUT) != HAL_OK)
			return 0;
		/* This loop will take a time. Insert rot_rdq() here for multitask environment. */
	} while ((token == 0xFF) && (HAL_GetTick() - tick < 200L));
	if(token != 0xFE) return 0;		/* Function fails if invalid DataStart token or timeout */

	/* MOSI line must be kept HIGH during data transfer from SD */
	memset(buff, 0xFF, btr);
	/* Store trailing data to the buffer */
	if(HAL_SPI_Receive_DMA(spi.hspi, buff, btr) != HAL_OK)
		return 0;
	while(HAL_SPI_GetState(spi.hspi) == HAL_SPI_STATE_BUSY_RX); /* Wait for the end of transfer */

	/* Discard CRC */
	token = 0xFF;
	for(BYTE n = 0; n < 2; n++)
		if(HAL_SPI_Transmit(spi.hspi, &token, 1, _FS_TIMEOUT) != HAL_OK)
			return 0;

	return 1; /* Function succeeded */
}

/**
 * @brief  Select the SD and wait for card ready
 * @retval : 1:OK, 0:Timeout
 */
static
int SPI_Select (void) {
  BYTE DUMMY_CLK = 0xFF;

  HAL_GPIO_WritePin(spi.SS_GPIO_Port, spi.SS_GPIO_Pin, GPIO_PIN_RESET);
  if(HAL_SPI_Transmit(spi.hspi, &DUMMY_CLK, 1, _FS_TIMEOUT) != HAL_OK)
    return 0;

  if (SPI_WaitReady(500)) return 1;  /* Wait for card ready */

  SPI_Deselect();

  return 0; /* Timeout */
}

/**
 * @brief  Wait for card ready
 * @param  wt: Timeout in ms
 * @retval : 1:Read, 0:Timeout
 */
static int SPI_WaitReady (UINT wt) {
  BYTE DUMMY_CLK[] = {0xFF, 0x0};
  uint32_t tick = HAL_GetTick();

  do {
    if(HAL_SPI_TransmitReceive(spi.hspi, DUMMY_CLK, DUMMY_CLK+1, 1, _FS_TIMEOUT) != HAL_OK)
    	return 0;
    /* Wait for card goes ready or timeout */
  } while ((DUMMY_CLK[1] != 0xFF) && ((HAL_GetTick() - tick) < wt ));
#if defined(_FS_DEBUG_LEVEL) && _FS_DEBUG_LEVEL == 3
  if (DUMMY_CLK[1] == 0xFF) {
    trace_printf("SPI_WaitReady: OK - Card Ready after %dms\n", (HAL_GetTick() - tick));
  } else {
    trace_printf("SPI_WaitReady: Timeout\n");
  }
#endif

  return (DUMMY_CLK[1] == 0xFF) ? 1 : 0;
}

#if _USE_WRITE

/**
 * @brief  Write a datablock (sector) to the SD
 * @param buff: Data buffer
 * param  token: Token
 * @retval : 1:OK, 0:Error
 */
static
int SPI_WriteDatablock (const BYTE *buff, BYTE token) {
	if (!SPI_WaitReady(500)) return 0;		/* Wait for card ready */

	if(HAL_SPI_Transmit(spi.hspi, &token, 1, _FS_TIMEOUT) != HAL_OK) { /* Send token */
		return 0;
	}

	if (token != 0xFD) { /* Send data if token is other than StopTran */
		if(HAL_SPI_Transmit_DMA(spi.hspi, (uint8_t*)buff, 512) == HAL_OK) { /* Data */
			while(HAL_SPI_GetState(spi.hspi) == HAL_SPI_STATE_BUSY_TX); /* Wait for the end of transfer */

			token = 0xFF;
			for(BYTE n = 0; n < 2; n++) /* Dummy CRC */
				if(HAL_SPI_Transmit(spi.hspi, &token, 1, _FS_TIMEOUT) != HAL_OK)
					return 0;

			if(HAL_SPI_TransmitReceive(spi.hspi, &token, &token, 1, _FS_TIMEOUT) == HAL_OK) { /* Receive data resp */
				if ((token & 0x1F) != 0x05) { /* Function fails if the data packet was not accepted */
					return 0;
				}
			}
		} else {
			return 0;
		}
	}
	return 1;
}
#endif

/**
 * @brief  Sends a command over the SPI bus
 * @param  cmd: Command index
 * @param  arg: 32-bit argument
 * @retval : R1 response (bit7==1:Failed to send)
 */
BYTE SPI_SendCMD(uint8_t cmd, uint32_t arg) {
  BYTE cmd_message[] = { cmd | 0x40, (BYTE) (arg >> 24), (BYTE) (arg >> 16),
                         (BYTE) (arg >> 8), (BYTE) (arg), 0x1 };
  BYTE res = 1 << 7;

  /* Send a CMD55 before sending an ACMD<n> command */
  if (cmd & 0x80) {
    cmd &= 0x7F;
    cmd_message[0] = cmd;
    res = SPI_SendCMD(CMD55, 0);
    if (res > 1) return res;
  }

  /* Select the card and wait for ready except to stop multiple block read */
  if (cmd != CMD12 && cmd != CMD0) {
    SPI_Deselect();
    if (!SPI_Select()) return 0xFF;
  }

  /* In some cards, the SPI_Select() will fail after a power-on-reset because
   * the card doesn't answer until a CMD0 is received. So, we simply assert the
   * SS and go on. */
  if (cmd == CMD0) {
    HAL_GPIO_WritePin(spi.SS_GPIO_Port, spi.SS_GPIO_Pin, GPIO_PIN_RESET);
  }

  /* CMD0 and CMD8 require a valid CRC-7 at the end of command */
  switch (cmd) {
  case CMD0:
    cmd_message[5] = 0x95; /* CRC for CMD0 */
    break;
  case CMD8:
    cmd_message[5] = 0x87; /* CRC for CMD8 */
    break;
  case CMD55:
    cmd_message[5] = 0x65; /* CRC for CMD55 */
    break;
  case (ACMD41 & 0x7F):
    cmd_message[5] = 0x77; /* CRC for CMD55 */
    break;
  }

#if defined(_FS_DEBUG_LEVEL) && _FS_DEBUG_LEVEL == 3
  trace_printf("Sending command: %d\n", cmd_message[0] & 0x3F);
#endif

  /* Send the command over the SPI bus */
  if(HAL_SPI_Transmit(spi.hspi, cmd_message, 6, _FS_TIMEOUT) != HAL_OK)
	  return res;

  if (cmd == CMD12) {
    cmd_message[0] = 0xFF;
    if(HAL_SPI_Transmit(spi.hspi, cmd_message, 1, _FS_TIMEOUT) != HAL_OK) /* Discard following one byte when CMD12 */
    	return res;
  }

  /* Receive command response waiting 10 bytes MAX */
  for (uint8_t i = 0; i < 10; i++) {
    cmd_message[0] = 0xFF;
    if(HAL_SPI_TransmitReceive(spi.hspi, cmd_message, cmd_message + 1, 1, _FS_TIMEOUT))
    	return res;

    if (!(cmd_message[1] & 0x80)) {
      res = cmd_message[1];
      break;
    }
  }

#if defined(_FS_DEBUG_LEVEL) && _FS_DEBUG_LEVEL == 3
  trace_printf("Received response: 0x%X\n", res);
#endif

  return res;
}

/**
 * @brief  Initializes a Drive
 * @param  pdrv: Physical drive number (0..)
 * @retval DSTATUS: Operation status
 */
DSTATUS SD_SPI_initialize(BYTE pdrv) {
  Stat = STA_NOINIT;
  uint8_t cmd, trials, DUMMY_DATA[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  uint32_t tick;

  if (pdrv) return STA_NOINIT;			/* This implementation supports only drive 0 */

  //FIXME: FCLK_SLOW();

  trials = 0;

  if (HAL_SPI_Transmit(spi.hspi, DUMMY_DATA, 10, _FS_TIMEOUT) == HAL_OK) { /* Sends 80 dummy clocks */
    while(SPI_SendCMD(CMD0, 0) != 0x1 && trials++ < 5) {
      SPI_Deselect();
      HAL_Delay(10);
    }
    if (trials < 5) { /* Put the card in SPI mode */
      if (SPI_SendCMD(CMD8, 0x1AA) == 0x1) { /* SDv2? */
    	/* Get 32 bit return value of R7 response */
        if (HAL_SPI_TransmitReceive(spi.hspi, DUMMY_DATA, DUMMY_DATA, 4, _FS_TIMEOUT) == HAL_OK) {
          if(DUMMY_DATA[2] == 0x1 && DUMMY_DATA[3] == 0xAA) { /* Does the card support VCC of 2.7-3.6V? */
            tick = HAL_GetTick();
            /* Wait for end of initialization with ACMD41(HCS) */
            while((HAL_GetTick() - tick) <= _FS_TIMEOUT && SPI_SendCMD(ACMD41, 1UL << 30));

            /* Check CCS bit in the OCR */
            if ((HAL_GetTick() - tick) < _FS_TIMEOUT && SPI_SendCMD(CMD58, 0) == 0) {
              if (HAL_SPI_TransmitReceive(spi.hspi, DUMMY_DATA, DUMMY_DATA, 4, _FS_TIMEOUT) == HAL_OK) {
                CardType = (DUMMY_DATA[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;  /* Card id SDv2 */
              }
            }
          }
        }
      }  else { /* Not SDv2 card */
        if (SPI_SendCMD(ACMD41, 0) <= 1)   { /* SDv1 or MMC? */
        	CardType = CT_SD1;
        	cmd = ACMD41;  /* SDv1 (ACMD41(0)) */
        } else {
        	CardType = CT_MMC;
        	cmd = CMD1;  /* MMCv3 (CMD1(0)) */
        }
        tick = HAL_GetTick();
        while((HAL_GetTick() - tick) <= _FS_TIMEOUT && SPI_SendCMD(cmd, 0));
        if (!((HAL_GetTick() - tick) <= _FS_TIMEOUT) || SPI_SendCMD(CMD16, 512) != 0) /* Set block length: 512 */
        	CardType = 0;
      }
    }
  }

  SPI_Deselect();

  if (CardType) {     /* OK */
	//FIXME:    FCLK_FAST();      /* Set fast clock */
    Stat &= ~STA_NOINIT;  /* Clear STA_NOINIT flag */
  }	else  { /* Failed */
    Stat = STA_NOINIT;
  }

  return Stat;
}

/**
 * @brief  Gets Disk Status
 * @param  pdrv: Physical drive number (0..)
 * @retval DSTATUS: Operation status
 */
DSTATUS SD_SPI_status(BYTE pdrv) {
  if(pdrv > 0) return STA_NOINIT;

  return Stat;
}

/**
 * @brief  Reads Sector(s)
 * @param  pdrv: Physical drive number (0..)
 * @param  *buff: Data buffer to store read data
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to read (1..128)
 * @retval DRESULT: Operation result
 */
DRESULT SD_SPI_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count) {
	if (pdrv || !count) return RES_PARERR;		/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check if drive is ready */

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* LBA or BA conversion (byte addressing cards) */

	if (count == 1) {	/* Single sector read */
		if ((SPI_SendCMD(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
			&& SPI_ReadDatablock(buff, 512))
			count = 0;
	}
	else {	/* Multiple sector read */
		if (SPI_SendCMD(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!SPI_ReadDatablock(buff, 512)) break;
				buff += 512;
			} while (--count);
			SPI_SendCMD(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}
	SPI_Deselect();

	return count ? RES_ERROR : RES_OK;	/* Return result */
}

/**
 * @brief  Writes Sector(s)
 * @param  pdrv: Physical drive number (0..)
 * @param  *buff: Data to be written
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to write (1..128)
 * @retval DRESULT: Operation result
 */
#if _USE_WRITE == 1
DRESULT SD_SPI_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count) {
	if (pdrv || !count) return RES_PARERR;		/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check drive status */
	if (Stat & STA_PROTECT) return RES_WRPRT;	/* Check write protect */

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* LBA ==> BA conversion (byte addressing cards) */

	if (count == 1) {	/* Single sector write */
		if ((SPI_SendCMD(CMD24, sector) == 0)	/* WRITE_BLOCK */
			&& SPI_WriteDatablock(buff, 0xFE))
			count = 0;
	}
	else {				/* Multiple sector write */
		if (CardType & CT_SDC) SPI_SendCMD(ACMD23, count);	/* Predefine number of sectors */
		if (SPI_SendCMD(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!SPI_WriteDatablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!SPI_WriteDatablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	SPI_Deselect();

	return count ? RES_ERROR : RES_OK;	/* Return result */
}
#endif /* _USE_WRITE == 1 */

/**
 * @brief  I/O control operation
 * @param  pdrv: Physical drive number (0..)
 * @param  cmd: Control code
 * @param  *buff: Buffer to send/receive control data
 * @retval DRESULT: Operation result
 */
#if _USE_IOCTL == 1
DRESULT SD_SPI_ioctl(BYTE pdrv, /* Physical drive nmuber (0..) */
BYTE cmd, /* Control code */
void *buff /* Buffer to send/receive control data */
) {
	DRESULT res;
	BYTE b, n, csd[16];
	DWORD *dp, st, ed, csize;


	if (pdrv) return RES_PARERR;					/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check if drive is ready */

	res = RES_ERROR;

	switch (cmd) {
	case CTRL_SYNC :		/* Wait for end of internal write process of the drive */
		if (SPI_Select()) res = RES_OK;
		break;

	case GET_SECTOR_COUNT :	/* Get drive capacity in unit of sector (DWORD) */
		if ((SPI_SendCMD(CMD9, 0) == 0) && SPI_ReadDatablock(csd, 16)) {
			if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
				csize = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
				*(DWORD*)buff = csize << 10;
			} else {					/* SDC ver 1.XX or MMC ver 3 */
				n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
				csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
				*(DWORD*)buff = csize << (n - 9);
			}
			res = RES_OK;
		}
		break;

	case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
		if (CardType & CT_SD2) {	/* SDC ver 2.00 */
			if (SPI_SendCMD(ACMD13, 0) == 0) {	/* Read SD status */
				b = 0xFF;
				if(HAL_SPI_Transmit(spi.hspi, &b, 1, _FS_TIMEOUT) != HAL_OK)
					return RES_ERROR;
				if (SPI_ReadDatablock(csd, 16)) {				/* Read partial block */
					for (n = 64 - 16; n; n--) {
						if(HAL_SPI_Transmit(spi.hspi, &b, 1, _FS_TIMEOUT) != HAL_OK)	/* Purge trailing data */
							return RES_ERROR;
					}
					*(DWORD*)buff = 16UL << (csd[10] >> 4);
					res = RES_OK;
				}
			}
		} else {					/* SDC ver 1.XX or MMC */
			if ((SPI_SendCMD(CMD9, 0) == 0) && SPI_ReadDatablock(csd, 16)) {	/* Read CSD */
				if (CardType & CT_SD1) {	/* SDC ver 1.XX */
					*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
				} else {					/* MMC */
					*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
				}
				res = RES_OK;
			}
		}
		break;

	case CTRL_TRIM :	/* Erase a block of sectors (used when _USE_ERASE == 1) */
		if (!(CardType & CT_SDC)) break;				/* Check if the card is SDC */
		if (SD_SPI_ioctl(pdrv, MMC_GET_CSD, csd)) break;	/* Get CSD */
		if (!(csd[0] >> 6) && !(csd[10] & 0x40)) break;	/* Check if sector erase can be applied to the card */
		dp = buff; st = dp[0]; ed = dp[1];				/* Load sector block */
		if (!(CardType & CT_BLOCK)) {
			st *= 512; ed *= 512;
		}
		if (SPI_SendCMD(CMD32, st) == 0 && SPI_SendCMD(CMD33, ed) == 0 && SPI_SendCMD(CMD38, 0) == 0 && SPI_WaitReady(30000))	/* Erase sector block */
			res = RES_OK;	/* FatFs does not check result of this command */
		break;

	default:
		res = RES_PARERR;
	}

	SPI_Deselect();

	return res;
}
#endif /* _USE_IOCTL == 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
