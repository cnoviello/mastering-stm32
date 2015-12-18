/**
  ******************************************************************************
  * @file    stm32l4xx_ll_comp.h
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    25-November-2015
  * @brief   Header file of COMP LL module.
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
#ifndef __STM32L4xx_LL_COMP_H
#define __STM32L4xx_LL_COMP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"

/** @addtogroup STM32L4xx_LL_Driver
  * @{
  */

#if defined (COMP1) || defined (COMP2)

/** @defgroup COMP_LL COMP
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup COMP_LL_Exported_Constants COMP Exported Constants
  * @{
  */


/** @defgroup COMP_LL_EC_POWERMODE POWERMODE
  * @{
  */
#define LL_COMP_POWERMODE_HIGHSPEED     ((uint32_t)0x00000000)                    /*!< High speed */
#define LL_COMP_POWERMODE_MEDIUMSPEED   (COMP_CSR_PWRMODE_0)                      /*!< Medium speed */
#define LL_COMP_POWERMODE_ULTRALOWPOWER (COMP_CSR_PWRMODE_1 | COMP_CSR_PWRMODE_0) /*!< Ultra-low power mode */
/**
  * @}
  */

/** @defgroup COMP_LL_EC_WINDOWMODE WINDOWMODE
  * @{
  */
#define LL_COMP_WINDOWMODE_DISABLE      ((uint32_t)0x00000000) /*!< Window mode disable: Comparators 1 and 2 are independent (no input connection) */
#define LL_COMP_WINDOWMODE_ENABLE       (COMP_CSR_WINMODE)     /*!< Window mode enable: Comparators 1 and 2 have their input non-inverting connected together. The common input to be used is the one of COMP2, COMP1 non-inverting input is no more accessible.  */
/**
  * @}
  */

/** @defgroup COMP_LL_EC_INVERTINGINPUT INVERTINGINPUT
  * @{
  */
#define LL_COMP_INVERTINGINPUT_1_4VREFINT (                                                            COMP_CSR_SCALEN | COMP_CSR_BRGEN) /*!< 1/4 VREFINT connected to comparator inverting input */
#define LL_COMP_INVERTINGINPUT_1_2VREFINT (                                        COMP_CSR_INMSEL_0 | COMP_CSR_SCALEN | COMP_CSR_BRGEN) /*!< 1/2 VREFINT connected to comparator inverting input */
#define LL_COMP_INVERTINGINPUT_3_4VREFINT (                    COMP_CSR_INMSEL_1                     | COMP_CSR_SCALEN | COMP_CSR_BRGEN) /*!< 3/4 VREFINT connected to comparator inverting input */
#define LL_COMP_INVERTINGINPUT_VREFINT    (                    COMP_CSR_INMSEL_1 | COMP_CSR_INMSEL_0 | COMP_CSR_SCALEN                 ) /*!< VREFINT connected to comparator1 inverting input */
#define LL_COMP_INVERTINGINPUT_DAC1       (COMP_CSR_INMSEL_2                                        ) /*!< DAC_OUT1 connected to comparator inverting input */
#define LL_COMP_INVERTINGINPUT_DAC2       (COMP_CSR_INMSEL_2                     | COMP_CSR_INMSEL_0) /*!< DAC_OUT2 connected to comparator inverting input */
#define LL_COMP_INVERTINGINPUT_IO1        (COMP_CSR_INMSEL_2 | COMP_CSR_INMSEL_1                    ) /*!< IO1 connected to comparator inverting input. On STM32L4: pin PB1 for COMP1, pin PB3 for COMP2. */
#define LL_COMP_INVERTINGINPUT_IO2        (COMP_CSR_INMSEL_2 | COMP_CSR_INMSEL_1 | COMP_CSR_INMSEL_0) /*!< IO2 connected to comparator inverting input. On STM32L4: pin PC4 for COMP1, pin PB7 for COMP2. */
/**
  * @}
  */

/** @defgroup COMP_LL_EC_NONINVERTINGINPUT NONINVERTINGINPUT
  * @{
  */
#define LL_COMP_NONINVERTINGINPUT_IO1   ((uint32_t)0x00000000) /*!< IO1 connected to comparator non inverting input. On STM32L4: pin PC5 for COMP1, pin PB4 for COMP2. */
#define LL_COMP_NONINVERTINGINPUT_IO2   (COMP_CSR_INPSEL)      /*!< IO2 connected to comparator non inverting input. On STM32L4: pin PB2 for COMP1, pin PB6 for COMP2. */
/**
  * @}
  */

/** @defgroup COMP_LL_EC_HYSTERESIS HYSTERESIS
  * @{
  */
#define LL_COMP_HYSTERESIS_NONE         ((uint32_t)0x00000000)              /*!< No hysteresis */
#define LL_COMP_HYSTERESIS_LOW          (                  COMP_CSR_HYST_0) /*!< Hysteresis level low */
#define LL_COMP_HYSTERESIS_MEDIUM       (COMP_CSR_HYST_1                  ) /*!< Hysteresis level medium */
#define LL_COMP_HYSTERESIS_HIGH         (COMP_CSR_HYST_1 | COMP_CSR_HYST_0) /*!< Hysteresis level high */
/**
  * @}
  */

/** @defgroup COMP_LL_EC_OUTPUTPOL OUTPUTPOL
  * @{
  */
#define LL_COMP_OUTPUTPOL_NONINVERTED   ((uint32_t)0x00000000) /*!< COMP output polarity is not inverted: comparator output is high when the non-inverting input is at a higher voltage than the inverting input */
#define LL_COMP_OUTPUTPOL_INVERTED      (COMP_CSR_POLARITY)    /*!< COMP output polarity is inverted: comparator output is low when the non-inverting input is at a lower voltage than the inverting input */
/**
  * @}
  */

/** @defgroup COMP_LL_EC_BLANKINGSRC BLANKINGSRC
  * @{
  */
#define LL_COMP_BLANKINGSRC_NONE        ((uint32_t)0x00000000) /*!< No blanking source */
#define LL_COMP_BLANKINGSRC_TIM1_OC5    (COMP_CSR_BLANKING_0)  /*!< Blanking source for COMP1: TIM1 OC5 selected as blanking source for comparator */
#define LL_COMP_BLANKINGSRC_TIM2_OC3    (COMP_CSR_BLANKING_1)  /*!< Blanking source for COMP1: TIM2 OC3 selected as blanking source for comparator */
#define LL_COMP_BLANKINGSRC_TIM3_OC3    (COMP_CSR_BLANKING_2)  /*!< Blanking source for COMP1: TIM3 OC3 selected as blanking source for compartor */
#define LL_COMP_BLANKINGSRC_TIM3_OC4    (COMP_CSR_BLANKING_0)  /*!< Blanking source for COMP2: TIM3 OC4 selected as blanking source for comparator */
#define LL_COMP_BLANKINGSRC_TIM8_OC5    (COMP_CSR_BLANKING_1)  /*!< Blanking source for COMP2: TIM8 OC5 selected as blanking source for comparator */
#define LL_COMP_BLANKINGSRC_TIM15_OC1   (COMP_CSR_BLANKING_2)  /*!< Blanking source for COMP2: TIM15 OC1 selected as blanking source for comparator */
/**
  * @}
  */

/** @defgroup COMP_LL_EC_OUTPUT_LEVEL OUTPUT LEVEL
  * @{
  */
#define LL_COMP_OUTPUT_LEVEL_LOW        (0x00000000)     /*!< Comparator output level low (if the polarity is not inverted, otherwise to be complemented) */
#define LL_COMP_OUTPUT_LEVEL_HIGH       (COMP_CSR_VALUE) /*!< Comparator output level high (if the polarity is not inverted, otherwise to be complemented) */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup COMP_LL_Exported_Macros COMP Exported Macros
  * @{
  */
/** @defgroup COMP_LL_EM_WRITE_READ Common Write and read registers macro
  * @{
  */
/**
  * @brief  Write a value in COMP register
  * @param  __INSTANCE__ comparator instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_COMP_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/**
  * @brief  Read a value in COMP register
  * @param  __INSTANCE__ comparator instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_COMP_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */


/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup COMP_LL_Exported_Functions COMP Exported Functions
  * @{
  */
/** @defgroup COMP_LL_EF_Configuration_comparator_modes Configuration of comparator modes
  * @{
  */
/**
  * @brief  Set comparator instance operating mode to adjust power and speed.
  * @rmtoll CSR     PWRMODE        LL_COMP_SetPowerMode
  * @param  COMPx Comparator instance
  * @param  PowerMode This parameter can be one of the following values:
  *         @arg @ref LL_COMP_POWERMODE_HIGHSPEED
  *         @arg @ref LL_COMP_POWERMODE_MEDIUMSPEED
  *         @arg @ref LL_COMP_POWERMODE_ULTRALOWPOWER
  * @retval None
  */
__STATIC_INLINE void LL_COMP_SetPowerMode(COMP_TypeDef *COMPx, uint32_t PowerMode)
{
  MODIFY_REG(COMPx->CSR, COMP_CSR_PWRMODE, PowerMode);
}

/**
  * @brief  Get comparator instance operating mode to adjust power and speed.
  * @rmtoll CSR     PWRMODE        LL_COMP_GetPowerMode
  * @param  COMPx Comparator instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_POWERMODE_HIGHSPEED
  *         @arg @ref LL_COMP_POWERMODE_MEDIUMSPEED
  *         @arg @ref LL_COMP_POWERMODE_ULTRALOWPOWER
  */
__STATIC_INLINE uint32_t LL_COMP_GetPowerMode(COMP_TypeDef *COMPx)
{
  return (uint32_t)(READ_BIT(COMPx->CSR, COMP_CSR_PWRMODE));
}

/**
  * @brief  Set window mode of the comparators.
  * @rmtoll CSR     WINMODE        LL_COMP_SetWindowMode
  * @param  COMPx Comparator instance.
  *         On this STM32 family, window mode can be set only from COMP instance: COMP2.
  * @param  WindowMode This parameter can be one of the following values:
  *         @arg @ref LL_COMP_WINDOWMODE_DISABLE
  *         @arg @ref LL_COMP_WINDOWMODE_ENABLE
  * @retval None
  */
__STATIC_INLINE void LL_COMP_SetWindowMode(COMP_TypeDef *COMPx, uint32_t WindowMode)
{
  MODIFY_REG(COMPx->CSR, COMP_CSR_WINMODE, WindowMode);
}

/**
  * @brief  Get window mode of the comparators.
  * @rmtoll CSR     WINMODE        LL_COMP_GetWindowMode
  * @param  COMPx Comparator instance
  *         On this STM32 family, window mode can be set only from COMP instance: COMP2.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_WINDOWMODE_DISABLE
  *         @arg @ref LL_COMP_WINDOWMODE_ENABLE
  */
__STATIC_INLINE uint32_t LL_COMP_GetWindowMode(COMP_TypeDef *COMPx)
{
  return (uint32_t)(READ_BIT(COMPx->CSR, COMP_CSR_WINMODE));
}

/**
  * @}
  */

/** @defgroup COMP_LL_EF_Configuration_comparator_inputs Configuration of comparator inputs
  * @{
  */
/**
  * @brief  Set COMP inputs inverting and non-inverting.
  * @note   In case of comparator input selected to be connected to IO:
  *         IP pins are specific to each comparator instance.
  *         Refer to description of parameters or to reference manual.
  * @note   On STM32L4, scaler bridge is configurable:
  *         to optimize power consumption, this function enables the
  *         voltage scaler bridge only when required (comparator input
  *         based on VrefInt).
  *         The scaler bridge is common for all comparator instances,
  *         therefore if at least one of the comparator instance
  *         is requiring the scaler bridge, it remains enabled.
  * @rmtoll CSR     INMSEL         LL_COMP_ConfigInputs\n
  *         CSR     INPSEL         LL_COMP_ConfigInputs\n
  *         CSR     BRGEN          LL_COMP_ConfigInputs\n
  *         CSR     SCALEN         LL_COMP_ConfigInputs
  * @param  COMPx Comparator instance
  * @param  InputInverting This parameter can be one of the following values:
  *         @arg @ref LL_COMP_INVERTINGINPUT_1_4VREFINT
  *         @arg @ref LL_COMP_INVERTINGINPUT_1_2VREFINT
  *         @arg @ref LL_COMP_INVERTINGINPUT_3_4VREFINT
  *         @arg @ref LL_COMP_INVERTINGINPUT_VREFINT
  *         @arg @ref LL_COMP_INVERTINGINPUT_DAC1
  *         @arg @ref LL_COMP_INVERTINGINPUT_DAC2
  *         @arg @ref LL_COMP_INVERTINGINPUT_IO1
  *         @arg @ref LL_COMP_INVERTINGINPUT_IO2
  * @param  InputNonInverting This parameter can be one of the following values:
  *         @arg @ref LL_COMP_NONINVERTINGINPUT_IO1
  *         @arg @ref LL_COMP_NONINVERTINGINPUT_IO2
  * @retval None
  */
__STATIC_INLINE void LL_COMP_ConfigInputs(COMP_TypeDef *COMPx, uint32_t InputInverting, uint32_t InputNonInverting)
{
  MODIFY_REG(COMPx->CSR, COMP_CSR_INMSEL | COMP_CSR_INPSEL | COMP_CSR_SCALEN | COMP_CSR_BRGEN, InputInverting | InputNonInverting);
}

/**
  * @brief  Set COMP inputs non-inverting.
  * @note   In case of comparator input selected to be connected to IO:
  *         IP pins are specific to each comparator instance.
  *         Refer to description of parameters or to reference manual.
  * @rmtoll CSR     INMSEL         LL_COMP_SetInputInverting
  * @param  COMPx Comparator instance
  * @param  InputNonInverting This parameter can be one of the following values:
  *         @arg @ref LL_COMP_NONINVERTINGINPUT_IO1
  *         @arg @ref LL_COMP_NONINVERTINGINPUT_IO2
  * @retval None
  */
__STATIC_INLINE void LL_COMP_SetInputNonInverting(COMP_TypeDef *COMPx, uint32_t InputNonInverting)
{
  MODIFY_REG(COMPx->CSR, COMP_CSR_INPSEL, InputNonInverting);
}

/**
  * @brief  Get COMP input non-inverting.
  * @note   In case of comparator input selected to be connected to IO:
  *         IP pins are specific to each comparator instance.
  *         Refer to description of parameters or to reference manual.
  * @rmtoll CSR     INMSEL         LL_COMP_GetInputInverting
  * @param  COMPx Comparator instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_NONINVERTINGINPUT_IO1
  *         @arg @ref LL_COMP_NONINVERTINGINPUT_IO2
  */
__STATIC_INLINE uint32_t LL_COMP_GetInputNonInverting(COMP_TypeDef *COMPx)
{
  return (uint32_t)(READ_BIT(COMPx->CSR, COMP_CSR_INPSEL));
}

/**
  * @brief  Set COMP input inverting.
  * @note   In case of comparator input selected to be connected to IO:
  *         IP pins are specific to each comparator instance.
  *         Refer to description of parameters or to reference manual.
  * @note   On STM32L4, scaler bridge is configurable:
  *         to optimize power consumption, this function enables the
  *         voltage scaler bridge only when required (comparator input
  *         based on VrefInt).
  *         The scaler bridge is common for all comparator instances,
  *         therefore if at least one of the comparator instance
  *         is requiring the scaler bridge, it remains enabled.
  * @rmtoll CSR     INPSEL         LL_COMP_SetInputNonInverting\n
  *         CSR     BRGEN          LL_COMP_SetInputNonInverting\n
  *         CSR     SCALEN         LL_COMP_SetInputNonInverting
  * @param  COMPx Comparator instance
  * @param  InputInverting This parameter can be one of the following values:
  *         @arg @ref LL_COMP_INVERTINGINPUT_1_4VREFINT
  *         @arg @ref LL_COMP_INVERTINGINPUT_1_2VREFINT
  *         @arg @ref LL_COMP_INVERTINGINPUT_3_4VREFINT
  *         @arg @ref LL_COMP_INVERTINGINPUT_VREFINT
  *         @arg @ref LL_COMP_INVERTINGINPUT_DAC1
  *         @arg @ref LL_COMP_INVERTINGINPUT_DAC2
  *         @arg @ref LL_COMP_INVERTINGINPUT_IO1
  *         @arg @ref LL_COMP_INVERTINGINPUT_IO2
  * @retval None
  */
__STATIC_INLINE void LL_COMP_SetInputInverting(COMP_TypeDef *COMPx, uint32_t InputInverting)
{
  MODIFY_REG(COMPx->CSR, COMP_CSR_INMSEL | COMP_CSR_SCALEN | COMP_CSR_BRGEN, InputInverting);
}

/**
  * @brief  Get COMP input inverting.
  * @note   In case of comparator input selected to be connected to IO:
  *         IP pins are specific to each comparator instance.
  *         Refer to description of parameters or to reference manual.
  * @rmtoll CSR     INPSEL         LL_COMP_GetInputNonInverting\n
  *         CSR     BRGEN          LL_COMP_GetInputNonInverting\n
  *         CSR     SCALEN         LL_COMP_GetInputNonInverting
  * @param  COMPx Comparator instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_INVERTINGINPUT_1_4VREFINT
  *         @arg @ref LL_COMP_INVERTINGINPUT_1_2VREFINT
  *         @arg @ref LL_COMP_INVERTINGINPUT_3_4VREFINT
  *         @arg @ref LL_COMP_INVERTINGINPUT_VREFINT
  *         @arg @ref LL_COMP_INVERTINGINPUT_DAC1
  *         @arg @ref LL_COMP_INVERTINGINPUT_DAC2
  *         @arg @ref LL_COMP_INVERTINGINPUT_IO1
  *         @arg @ref LL_COMP_INVERTINGINPUT_IO2
  */
__STATIC_INLINE uint32_t LL_COMP_GetInputInverting(COMP_TypeDef *COMPx)
{
  return (uint32_t)(READ_BIT(COMPx->CSR, COMP_CSR_INMSEL | COMP_CSR_SCALEN | COMP_CSR_BRGEN));
}

/**
  * @brief  Set comparator instance hysteresis mode of the inverting input.
  * @rmtoll CSR     HYST           LL_COMP_SetInputHysteresis
  * @param  COMPx Comparator instance
  * @param  Hysteresis This parameter can be one of the following values:
  *         @arg @ref LL_COMP_HYSTERESIS_NONE
  *         @arg @ref LL_COMP_HYSTERESIS_LOW
  *         @arg @ref LL_COMP_HYSTERESIS_MEDIUM
  *         @arg @ref LL_COMP_HYSTERESIS_HIGH
  * @retval None
  */
__STATIC_INLINE void LL_COMP_SetInputHysteresis(COMP_TypeDef *COMPx, uint32_t Hysteresis)
{
  MODIFY_REG(COMPx->CSR, COMP_CSR_HYST, Hysteresis);
}

/**
  * @brief  Get comparator instance hysteresis mode of the inverting input.
  * @rmtoll CSR     HYST           LL_COMP_GetInputHysteresis
  * @param  COMPx Comparator instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_HYSTERESIS_NONE
  *         @arg @ref LL_COMP_HYSTERESIS_LOW
  *         @arg @ref LL_COMP_HYSTERESIS_MEDIUM
  *         @arg @ref LL_COMP_HYSTERESIS_HIGH
  */
__STATIC_INLINE uint32_t LL_COMP_GetInputHysteresis(COMP_TypeDef *COMPx)
{
  return (uint32_t)(READ_BIT(COMPx->CSR, COMP_CSR_HYST));
}

/**
  * @}
  */

/** @defgroup COMP_LL_EF_Configuration_comparator_output Configuration of comparator output
  * @{
  */
/**
  * @brief  Set comparator instance output polarity.
  * @rmtoll CSR     POLARITY       LL_COMP_SetOutputPolarity
  * @param  COMPx Comparator instance
  * @param  Polarity This parameter can be one of the following values:
  *         @arg @ref LL_COMP_OUTPUTPOL_NONINVERTED
  *         @arg @ref LL_COMP_OUTPUTPOL_INVERTED
  * @retval None
  */
__STATIC_INLINE void LL_COMP_SetOutputPolarity(COMP_TypeDef *COMPx, uint32_t Polarity)
{
  MODIFY_REG(COMPx->CSR, COMP_CSR_POLARITY, Polarity);
}

/**
  * @brief  Get comparator instance output polarity.
  * @rmtoll CSR     POLARITY       LL_COMP_GetOutputPolarity
  * @param  COMPx Comparator instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_OUTPUTPOL_NONINVERTED
  *         @arg @ref LL_COMP_OUTPUTPOL_INVERTED
  */
__STATIC_INLINE uint32_t LL_COMP_GetOutputPolarity(COMP_TypeDef *COMPx)
{
  return (uint32_t)(READ_BIT(COMPx->CSR, COMP_CSR_POLARITY));
}

/**
  * @brief  Set comparator instance blanking source.
  * @note   Blanking source may be specific to each comparator instance.
  *         Refer to description of parameters or to reference manual.
  * @rmtoll CSR     BLANKING       LL_COMP_SetOutputBlankingSource
  * @param  COMPx Comparator instance
  * @param  BlankingSource This parameter can be one of the following values:
  *         @arg @ref LL_COMP_BLANKINGSRC_NONE
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM1_OC5  (1)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM2_OC3  (1)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM3_OC3  (1)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM3_OC4  (2)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM8_OC5  (2)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM15_OC1 (2)
  *         (1) On STM32L4, parameter available only on comparator instance: COMP1.
  *         (2) On STM32L4, parameter available only on comparator instance: COMP2.
  * @retval None
  */
__STATIC_INLINE void LL_COMP_SetOutputBlankingSource(COMP_TypeDef *COMPx, uint32_t BlankingSource)
{
  MODIFY_REG(COMPx->CSR, COMP_CSR_BLANKING, BlankingSource);
}

/**
  * @brief  Get comparator instance blanking source.
  * @note   Blanking source may be specific to each comparator instance.
  *         Refer to description of parameters or to reference manual.
  * @rmtoll CSR     BLANKING       LL_COMP_GetOutputBlankingSource
  * @param  COMPx Comparator instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_BLANKINGSRC_NONE
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM1_OC5  (1)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM2_OC3  (1)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM3_OC3  (1)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM3_OC4  (2)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM8_OC5  (2)
  *         @arg @ref LL_COMP_BLANKINGSRC_TIM15_OC1 (2)
  *         (1) On STM32L4, parameter available only on comparator instance: COMP1.
  *         (2) On STM32L4, parameter available only on comparator instance: COMP2.
  */
__STATIC_INLINE uint32_t LL_COMP_GetOutputBlankingSource(COMP_TypeDef *COMPx)
{
  return (uint32_t)(READ_BIT(COMPx->CSR, COMP_CSR_BLANKING));
}

/**
  * @}
  */

/** @defgroup COMP_LL_EF_Operation Operation
  * @{
  */
/**
  * @brief  Enable comparator instance.
  * @rmtoll CSR     EN             LL_COMP_Enable
  * @param  COMPx Comparator instance
  * @retval None
  */
__STATIC_INLINE void LL_COMP_Enable(COMP_TypeDef *COMPx)
{
  SET_BIT(COMPx->CSR, COMP_CSR_EN);
}

/**
  * @brief  Disable comparator instance.
  * @rmtoll CSR     EN             LL_COMP_Disable
  * @param  COMPx Comparator instance
  * @retval None
  */
__STATIC_INLINE void LL_COMP_Disable(COMP_TypeDef *COMPx)
{
  CLEAR_BIT(COMPx->CSR, COMP_CSR_EN);
}

/**
  * @brief  Get COMP enable state
  *         (0: COMP is disabled, 1: COMP is enabled)
  * @rmtoll CSR     EN             LL_COMP_IsEnabled
  * @param  COMPx Comparator instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_COMP_IsEnabled(COMP_TypeDef *COMPx)
{
  return (READ_BIT(COMPx->CSR, COMP_CSR_EN) == (COMP_CSR_EN));
}

/**
  * @brief  Lock comparator instance.
  *         Once locked, comparator configuration can be accessed in read-only.
  *         The only way to unlock the comparator is a device hardware reset.
  * @rmtoll CSR     LOCK           LL_COMP_Lock
  * @param  COMPx Comparator instance
  * @retval None
  */
__STATIC_INLINE void LL_COMP_Lock(COMP_TypeDef *COMPx)
{
  SET_BIT(COMPx->CSR, COMP_CSR_LOCK);
}

/**
  * @brief  Get COMP lock state.
  *         (0: COMP is unlocked, 1: COMP is locked)
  * @rmtoll CSR     LOCK           LL_COMP_IsLocked
  * @param  COMPx Comparator instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_COMP_IsLocked(COMP_TypeDef *COMPx)
{
  return (READ_BIT(COMPx->CSR, COMP_CSR_LOCK) == (COMP_CSR_LOCK));
}

/**
  * @brief  Read comparator instance output level.
  *         The output level depends on the selected polarity.
  *         If the polarity is not inverted:
  *          - Comparator output is low when the non-inverting input is at a lower
  *            voltage than the inverting input
  *          - Comparator output is high when the non-inverting input is at a higher
  *            voltage than the inverting input
  *         If the polarity is inverted:
  *          - Comparator output is high when the non-inverting input is at a lower
  *            voltage than the inverting input
  *          - Comparator output is low when the non-inverting input is at a higher
  *            voltage than the inverting input
  * @rmtoll CSR     VALUE          LL_COMP_ReadOutputLevel
  * @param  COMPx Comparator instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_COMP_OUTPUT_LEVEL_LOW
  *         @arg @ref LL_COMP_OUTPUT_LEVEL_HIGH
  */
__STATIC_INLINE uint32_t LL_COMP_ReadOutputLevel(COMP_TypeDef *COMPx)
{
  return (uint32_t)(READ_BIT(COMPx->CSR, COMP_CSR_VALUE));
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

#endif /* COMP1 || COMP2 */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32L4xx_LL_COMP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
