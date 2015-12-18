/**
  ******************************************************************************
  * @file    stm32l4xx_ll_utils.h
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    25-November-2015
  * @brief   Header file of UTILS LL module.
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    [..]
    The LL UTILS driver contains a set of generic APIs that can be
    used by user:
      (+) Device electronic signature
      (+) Timing functions

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L4xx_LL_UTILS_H
#define __STM32L4xx_LL_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"

/** @addtogroup STM32L4xx_LL_Driver
  * @{
  */

/** @defgroup UTILS_LL UTILS
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/** @defgroup UTILS_LL_Private_Constants UTILS Private Constants
  * @{
  */

/* Max delay can be used in LL_mDelay */
#define LL_MAX_DELAY                  (uint32_t)0xFFFFFFFF

/**
 * @brief Unique device ID register base address
 */
#define UID_BASE_ADDRESS              (uint32_t)0x1FFF7590

/**
 * @brief Flash size data register base address
 */
#define FLASHSIZE_BASE_ADDRESS        (uint32_t)0x1FFF75E0

/**
 * @brief Package data register base address
 */
#define PACKAGESIZE_BASE_ADDRESS      (uint32_t)0x1FFF7500

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup UTILS_LL_Exported_Constants UTILS Exported Constants
  * @{
  */

/** @defgroup UTILS_EC_PACKAGETYPE PACKAGE TYPE
  * @{
  */
#define LL_UTILS_PACKAGETYPE_LQFP64         (uint32_t)0x00000000   /*!< LQFP64 package type                      */
#define LL_UTILS_PACKAGETYPE_LQPF100        (uint32_t)0x00000002   /*!< LQFP100 package type                     */
#define LL_UTILS_PACKAGETYPE_BGA132         (uint32_t)0x00000003   /*!< BGA132 package type                      */
#define LL_UTILS_PACKAGETYPE_LQFP144_CSP72  (uint32_t)0x00000004   /*!< LQFP144, WLCSP81 or WLCSP72 package type */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup UTILS_LL_Exported_Functions UTILS Exported Functions
  * @{
  */

/** @defgroup UTILS_EF_DEVICE_ELECTRONIC_SIGNATURE DEVICE ELECTRONIC SIGNATURE
  * @{
  */

/**
  * @brief  Get Word0 of the unique device identifier (UID based on 96 bits)
  * @retval UID[31:0]: X and Y coordinates on the wafer expressed in BCD format
  */
__STATIC_INLINE uint32_t LL_GetUID_Word0(void)
{
  return (uint32_t)(READ_REG(*((uint32_t *)UID_BASE_ADDRESS)));
}

/**
  * @brief  Get Word1 of the unique device identifier (UID based on 96 bits)
  * @retval UID[63:32]: Wafer number (UID[39:32]) & LOT_NUM[23:0] (UID[63:40])
  */
__STATIC_INLINE uint32_t LL_GetUID_Word1(void)
{
  return (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE_ADDRESS + 4))));
}

/**
  * @brief  Get Word2 of the unique device identifier (UID based on 96 bits)
  * @retval UID[95:64]: Lot number (ASCII encoded) - LOT_NUM[31:24]
  */
__STATIC_INLINE uint32_t LL_GetUID_Word2(void)
{
  return (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE_ADDRESS + 8))));
}

/**
  * @brief  Get Flash memory size
  * @note   This bitfield indicates the size of the device Flash memory expressed in
  *         Kbytes. As an example, 0x040 corresponds to 64 Kbytes.
  * @retval FLASH_SIZE[15:0]: Flash memory size
  */
__STATIC_INLINE uint32_t LL_GetFlashSize(void)
{
  return (uint16_t)(READ_REG(*((uint32_t *)FLASHSIZE_BASE_ADDRESS)));
}

/**
  * @brief  Get Package type
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_UTILS_PACKAGETYPE_LQFP64
  *         @arg @ref LL_UTILS_PACKAGETYPE_LQPF100
  *         @arg @ref LL_UTILS_PACKAGETYPE_BGA132
  *         @arg @ref LL_UTILS_PACKAGETYPE_LQFP144_CSP72
  */
__STATIC_INLINE uint32_t LL_GetPackageType(void)
{
  return (uint8_t)(READ_REG(*((uint32_t *)PACKAGESIZE_BASE_ADDRESS)) & (uint32_t)0x1F);
}

/**
  * @}
  */

/** @defgroup UTILS_EF_DELAY DELAY
  * @{
  */

/**
  * @brief  This function provides accurate delay (in milliseconds) based
  *         on SysTick counter flag
  * @note   To respect 1ms timebase, user should call LL_InitTick function which
  *         will configure Systick to 1ms
  * @param  Delay specifies the delay time length, in milliseconds.
  * @retval None
  */
__STATIC_INLINE void LL_mDelay(uint32_t Delay)
{
  volatile uint32_t  tmp = SysTick->CTRL;  /* Clear the COUNTFLAG first */
  ((void)tmp);

  /* Add a period to guaranty minimum wait */
  if (Delay < LL_MAX_DELAY)
  {
    Delay++;
  }

  while (Delay)
  {
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0)
    {
      Delay--;
    }
  }
}

/**
  * @brief  This function configures the source of the time base.
  * @note   The time source is configured to have 1ms time base.
  * @param  HCLKFrequency HCLK frequency in Hz (can be calculated thanks to RCC helper macro)
  * @retval None
  */
__STATIC_INLINE void LL_Init1msTick(uint32_t HCLKFrequency)
{
  /* Configure the SysTick to have interrupt in 1ms time base */
  SysTick->LOAD  = (uint32_t)((HCLKFrequency / 1000) - 1UL);  /* set reload register */
  SysTick->VAL   = 0UL;                                       /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_ENABLE_Msk;                   /* Enable the Systick Timer */
}

/**
  * @}
  */

/** @defgroup UTILS_EF_SYSTEM SYSTEM
  * @{
  */

/**
  * @brief  This function sets directly SystemCoreClock CMSIS variable.
  * @note   Variable can be calculated also through SystemCoreClockUpdate function.
  * @param  HCLKFrequency HCLK frequency in Hz (can be calculated thanks to RCC helper macro)
  * @retval None
  */
__STATIC_INLINE void LL_SetSystemCoreClock(uint32_t HCLKFrequency)
{
  /* HCLK clock frequency */
  SystemCoreClock = HCLKFrequency;
}

/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32L4xx_LL_UTILS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
