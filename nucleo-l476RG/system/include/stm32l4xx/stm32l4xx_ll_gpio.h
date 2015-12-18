/**
  ******************************************************************************
  * @file    stm32l4xx_ll_gpio.h
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    25-November-2015
  * @brief   Header file of GPIO LL module.
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
#ifndef __STM32L4xx_LL_GPIO_H
#define __STM32L4xx_LL_GPIO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"

/** @addtogroup STM32L4xx_LL_Driver
  * @{
  */

#if defined (GPIOA) || defined (GPIOB) || defined (GPIOC) || defined (GPIOD) || defined (GPIOE) || defined (GPIOF) || defined (GPIOG) || defined (GPIOH)
  
/** @defgroup GPIO_LL GPIO
  * @{
  */
  
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup GPIO_LL_Private_Constants GPIO Private Constants
  * @{
  */
#if defined(GPIO_ASCR_ASC0)
#define ANALOG_SWITCH_CONTROL
#endif

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/ 
/* Exported constants --------------------------------------------------------*/
/** @defgroup GPIO_LL_Exported_Constants GPIO Exported Constants
  * @{
  */

/** @defgroup GPIO_LL_EC_PIN PIN
  * @{
  */
#define LL_GPIO_PIN_0                      GPIO_BSRR_BS0 /*!< Select pin 0 */
#define LL_GPIO_PIN_1                      GPIO_BSRR_BS1 /*!< Select pin 1 */
#define LL_GPIO_PIN_2                      GPIO_BSRR_BS2 /*!< Select pin 2 */
#define LL_GPIO_PIN_3                      GPIO_BSRR_BS3 /*!< Select pin 3 */
#define LL_GPIO_PIN_4                      GPIO_BSRR_BS4 /*!< Select pin 4 */
#define LL_GPIO_PIN_5                      GPIO_BSRR_BS5 /*!< Select pin 5 */
#define LL_GPIO_PIN_6                      GPIO_BSRR_BS6 /*!< Select pin 6 */
#define LL_GPIO_PIN_7                      GPIO_BSRR_BS7 /*!< Select pin 7 */
#define LL_GPIO_PIN_8                      GPIO_BSRR_BS8 /*!< Select pin 8 */
#define LL_GPIO_PIN_9                      GPIO_BSRR_BS9 /*!< Select pin 9 */
#define LL_GPIO_PIN_10                     GPIO_BSRR_BS10 /*!< Select pin 10 */
#define LL_GPIO_PIN_11                     GPIO_BSRR_BS11 /*!< Select pin 11 */
#define LL_GPIO_PIN_12                     GPIO_BSRR_BS12 /*!< Select pin 12 */
#define LL_GPIO_PIN_13                     GPIO_BSRR_BS13 /*!< Select pin 13 */
#define LL_GPIO_PIN_14                     GPIO_BSRR_BS14 /*!< Select pin 14 */
#define LL_GPIO_PIN_15                     GPIO_BSRR_BS15 /*!< Select pin 15 */
#define LL_GPIO_PIN_ALL                    (GPIO_BSRR_BS0 | GPIO_BSRR_BS1 | GPIO_BSRR_BS2 | \
                                           GPIO_BSRR_BS3 |GPIO_BSRR_BS4 | GPIO_BSRR_BS5 | \
                                           GPIO_BSRR_BS6 | GPIO_BSRR_BS7 | GPIO_BSRR_BS8 | \
                                           GPIO_BSRR_BS9 | GPIO_BSRR_BS10 | GPIO_BSRR_BS11 | \
                                           GPIO_BSRR_BS12 | GPIO_BSRR_BS13 | GPIO_BSRR_BS14 | \
                                           GPIO_BSRR_BS15) /*!< Select all pins */
/**
  * @}
  */

/** @defgroup GPIO_LL_EC_MODE MODE
  * @{
  */
#define LL_GPIO_MODE_INPUT                 ((uint32_t)0x00000000) /*!< Select input mode */
#define LL_GPIO_MODE_OUTPUT                GPIO_MODER_MODE0_0 /*!< Select output mode */
#define LL_GPIO_MODE_ALTERNATE             GPIO_MODER_MODE0_1 /*!< Select alternate function mode */
#define LL_GPIO_MODE_ANALOG                GPIO_MODER_MODE0 /*!< Select analog mode */ 
/**
  * @}
  */

/** @defgroup GPIO_LL_EC_OUTPUT OUTPUT
  * @{
  */
#define LL_GPIO_OUTPUT_PUSHPULL            ((uint32_t)0x00000000) /*!< Select push-pull as output type */
#define LL_GPIO_OUTPUT_OPENDRAIN           GPIO_IDR_ID0 /*!< Select open-drain as output type */
/**
  * @}
  */

/** @defgroup GPIO_LL_EC_SPEED SPEED
  * @{
  */
#define LL_GPIO_SPEED_LOW                  ((uint32_t)0x00000000) /*!< Select I/O low output speed */
#define LL_GPIO_SPEED_MEDIUM               GPIO_OSPEEDR_OSPEED0_0 /*!< Select I/O medium output speed */
#define LL_GPIO_SPEED_FAST                 GPIO_OSPEEDR_OSPEED0_1 /*!< Select I/O fast output speed */
#define LL_GPIO_SPEED_HIGH                 GPIO_OSPEEDR_OSPEED0 /*!< Select I/O high output speed */
/**
  * @}
  */

/** @defgroup GPIO_LL_EC_PULL PULL
  * @{
  */
#define LL_GPIO_PULL_NO                    ((uint32_t)0x00000000) /*!< Select I/O no pull */
#define LL_GPIO_PULL_UP                    GPIO_PUPDR_PUPD0_0 /*!< Select I/O pull up */
#define LL_GPIO_PULL_DOWN                  GPIO_PUPDR_PUPD0_1 /*!< Select I/O pull down */
/**
  * @}
  */

/** @defgroup GPIO_LL_EC_AF AF
  * @{
  */
#define LL_GPIO_AF_0                       ((uint32_t)0x0000000) /*!< Select alternate function 0 */
#define LL_GPIO_AF_1                       ((uint32_t)0x0000001) /*!< Select alternate function 1 */
#define LL_GPIO_AF_2                       ((uint32_t)0x0000002) /*!< Select alternate function 2 */
#define LL_GPIO_AF_3                       ((uint32_t)0x0000003) /*!< Select alternate function 3 */
#define LL_GPIO_AF_4                       ((uint32_t)0x0000004) /*!< Select alternate function 4 */
#define LL_GPIO_AF_5                       ((uint32_t)0x0000005) /*!< Select alternate function 5 */
#define LL_GPIO_AF_6                       ((uint32_t)0x0000006) /*!< Select alternate function 6 */
#define LL_GPIO_AF_7                       ((uint32_t)0x0000007) /*!< Select alternate function 7 */
#define LL_GPIO_AF_8                       ((uint32_t)0x0000008) /*!< Select alternate function 8 */
#define LL_GPIO_AF_9                       ((uint32_t)0x0000009) /*!< Select alternate function 9 */
#define LL_GPIO_AF_10                      ((uint32_t)0x000000A) /*!< Select alternate function 10 */
#define LL_GPIO_AF_11                      ((uint32_t)0x000000B) /*!< Select alternate function 11 */
#define LL_GPIO_AF_12                      ((uint32_t)0x000000C) /*!< Select alternate function 12 */
#define LL_GPIO_AF_13                      ((uint32_t)0x000000D) /*!< Select alternate function 13 */
#define LL_GPIO_AF_14                      ((uint32_t)0x000000E) /*!< Select alternate function 14 */
#define LL_GPIO_AF_15                      ((uint32_t)0x000000F) /*!< Select alternate function 15 */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup GPIO_LL_Exported_Macros GPIO Exported Macros
  * @{
  */

/** @defgroup GPIO_LL_EM_WRITE_READ Common Write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in GPIO register
  * @param  __INSTANCE__ GPIO Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_GPIO_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/**
  * @brief  Read a value in GPIO register
  * @param  __INSTANCE__ GPIO Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_GPIO_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup GPIO_LL_Exported_Functions GPIO Exported Functions
  * @{
  */

/** @defgroup GPIO_LL_EF_Port_Configuration Port_Configuration
  * @{
  */

/**
  * @brief  Configure gpio mode for a dedicated pin on dedicated port. 
  *         I/O mode can be one of following: 
  *         - Input mode
  *         - General purpose output
  *         - Alternate function mode
  *         - Analog
  * @note   Warning: only one pin can be passed as parameter.
  * @note   The debug pins are in AF after reset:
  *         - PA15: JTDI
  *         - PA14: JTCK/SWCLK
  *         - PA13: JTMS/SWDAT
  *         - PB4: NJTRST
  *         - PB3: JTDO
  * @rmtoll MODER        MODEy         LL_GPIO_SetPinMode
  * @param  GPIOx GPIO Port
  * @param  Pin This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  * @param  Mode This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_MODE_INPUT
  *         @arg @ref LL_GPIO_MODE_OUTPUT
  *         @arg @ref LL_GPIO_MODE_ALTERNATE
  *         @arg @ref LL_GPIO_MODE_ANALOG
  * @retval None
  */
__STATIC_INLINE void LL_GPIO_SetPinMode(GPIO_TypeDef* GPIOx, uint32_t Pin, uint32_t Mode)
{
  MODIFY_REG(GPIOx->MODER, (GPIO_MODER_MODE0 << (POSITION_VAL(Pin) * 2)), (Mode << (POSITION_VAL(Pin) * 2)));
}

/**
  * @brief  Return gpio mode for a dedicated pin on dedicated port. 
  *         I/O mode can be one of following: 
  *         - Input mode
  *         - General purpose output
  *         - Alternate function mode
  *         - Analog
  * @note   Warning: only one pin can be passed as parameter.
  * @rmtoll MODER        MODEy         LL_GPIO_GetPinMode
  * @param  GPIOx GPIO Port
  * @param  Pin This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  * @retval Gpio pin mode
  */
__STATIC_INLINE uint32_t LL_GPIO_GetPinMode(GPIO_TypeDef* GPIOx, uint32_t Pin)
{
  return (uint32_t)(READ_BIT(GPIOx->MODER, (GPIO_MODER_MODE0 << (POSITION_VAL(Pin) * 2))));
}

/**
  * @brief  Configure gpio output type for several pins on dedicated port. 
  *         Output type as to be set when gpio pin is in output or
  *         alternate modes. Possible type are :
  *         - Push-pull 
  *         - Open-drain
  * @rmtoll OTYPER       OTy           LL_GPIO_SetPinOutputType
  * @param  GPIOx GPIO Port 
  * @param  PinMask This parameter can be a combination of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  *         @arg @ref LL_GPIO_PIN_ALL
  * @param  OutputType This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_OUTPUT_PUSHPULL
  *         @arg @ref LL_GPIO_OUTPUT_OPENDRAIN
  * @retval None
  */
__STATIC_INLINE void LL_GPIO_SetPinOutputType(GPIO_TypeDef* GPIOx, uint32_t PinMask, uint32_t OutputType)
{
  MODIFY_REG(GPIOx->OTYPER, PinMask, (PinMask * OutputType));
}

/**
  * @brief  Return gpio output type for several pins on dedicated port. 
  *         Output type as to be set when gpio pin is in output or
  *         alternate modes. Possible type are :
  *         - Push-pull 
  *         - Open-drain
  * @rmtoll OTYPER       OTy           LL_GPIO_GetPinOutputType
  * @param  GPIOx GPIO Port 
  * @param  PinMask This parameter can be a combination of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  *         @arg @ref LL_GPIO_PIN_ALL
  * @retval Gpio pin output type
  */
__STATIC_INLINE uint32_t LL_GPIO_GetPinOutputType(GPIO_TypeDef* GPIOx, uint32_t PinMask)
{
  return (uint32_t)(READ_BIT(GPIOx->OTYPER, PinMask));
}

/**
  * @brief  Configure gpio speed for a dedicated pin on dedicated port. 
  *         I/O speed can be one of following: 
  *         - Low speed
  *         - Medium speed
  *         - Fast speed
  *         - High speed
  * @note   Warning: only one pin can be passed as parameter.
  * @note   Refer to datasheet for frequency specifications and the power
  *         supply and load conditions for each speed.
  * @rmtoll OSPEEDR      OSPEEDy       LL_GPIO_SetPinSpeed
  * @param  GPIOx GPIO Port
  * @param  Pin This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  * @param  Speed This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_SPEED_LOW
  *         @arg @ref LL_GPIO_SPEED_MEDIUM
  *         @arg @ref LL_GPIO_SPEED_FAST
  *         @arg @ref LL_GPIO_SPEED_HIGH
  * @retval None
  */
__STATIC_INLINE void LL_GPIO_SetPinSpeed(GPIO_TypeDef* GPIOx, uint32_t Pin, uint32_t  Speed)
{
  MODIFY_REG(GPIOx->OSPEEDR, (GPIO_OSPEEDR_OSPEED0 << (POSITION_VAL(Pin) * 2)), (Speed<< (POSITION_VAL(Pin) * 2)));
}

/**
  * @brief  Return gpio speed for a dedicated pin on dedicated port. 
  *         I/O speed can be one of following: 
  *         - Low speed
  *         - Medium speed
  *         - Fast speed
  *         - High speed
  * @note   Warning: only one pin can be passed as parameter.
  * @note   Refer to datasheet for frequency specifications and the power
  *         supply and load conditions for each speed.
  * @rmtoll OSPEEDR      OSPEEDy       LL_GPIO_GetPinSpeed
  * @param  GPIOx GPIO Port
  * @param  Pin This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  * @retval Gpio pin speed
  */
__STATIC_INLINE uint32_t LL_GPIO_GetPinSpeed(GPIO_TypeDef* GPIOx, uint32_t Pin)
{
  return (uint32_t)(READ_BIT(GPIOx->OSPEEDR, (GPIO_OSPEEDR_OSPEED0 << (POSITION_VAL(Pin) * 2))));
}

/**
  * @brief  Configure gpio pull-up or pull-down for a dedicated pin on a dedicated
  *         port
  * @note   Warning: only one pin can be passed as parameter.
  * @note   The debug pins are pull-up/pull-down after reset:
  *         - PA15: JTDI in pull-up
  *         - PA14: JTCK/SWCLK in pull-down
  *         - PA13: JTMS/SWDAT in pull-up
  *         - PB4: NJTRST in pull-up
  *         - PB3: JTDO in floating stateno pull-up/pull-down
  * @rmtoll PUPDR        PUPDy         LL_GPIO_SetPinPull
  * @param  GPIOx GPIO Port
  * @param  Pin This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  * @param  Pull This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_PULL_NO
  *         @arg @ref LL_GPIO_PULL_UP
  *         @arg @ref LL_GPIO_PULL_DOWN
  * @retval None
  */
__STATIC_INLINE void LL_GPIO_SetPinPull(GPIO_TypeDef* GPIOx, uint32_t Pin, uint32_t Pull)
{
  MODIFY_REG(GPIOx->PUPDR, (GPIO_PUPDR_PUPD0 << (POSITION_VAL(Pin) * 2)), (Pull << (POSITION_VAL(Pin) * 2)));
}

/**
  * @brief  Return gpio pull-up or pull-down for a dedicated pin on a dedicated
  *         port
  * @note   Warning: only one pin can be passed as parameter.
  * @rmtoll PUPDR        PUPDy         LL_GPIO_GetPinPull
  * @param  GPIOx GPIO Port
  * @param  Pin This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  * @retval Gpio pin pull value
  */
__STATIC_INLINE uint32_t LL_GPIO_GetPinPull(GPIO_TypeDef* GPIOx, uint32_t Pin)
{
  return (uint32_t)(READ_BIT(GPIOx->PUPDR, (GPIO_PUPDR_PUPD0 << (POSITION_VAL(Pin) * 2))));
}

/**
  * @brief  Configure gpio alternate function for a dedicated pin from 0 to 7
  *         included, of a dedicated port. Possible values are from AF0 to
  *         AF15 depending on target.
  * @note   Warning: only one pin can be passed as parameter.
  * @rmtoll AFRL         AFSELy        LL_GPIO_SetAFPin_0_7
  * @param  GPIOx GPIO Port
  * @param  Pin This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  * @param  Alternate This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_AF_0
  *         @arg @ref LL_GPIO_AF_1
  *         @arg @ref LL_GPIO_AF_2
  *         @arg @ref LL_GPIO_AF_3
  *         @arg @ref LL_GPIO_AF_4
  *         @arg @ref LL_GPIO_AF_5
  *         @arg @ref LL_GPIO_AF_6
  *         @arg @ref LL_GPIO_AF_7
  *         @arg @ref LL_GPIO_AF_8
  *         @arg @ref LL_GPIO_AF_9
  *         @arg @ref LL_GPIO_AF_10
  *         @arg @ref LL_GPIO_AF_11
  *         @arg @ref LL_GPIO_AF_12
  *         @arg @ref LL_GPIO_AF_13
  *         @arg @ref LL_GPIO_AF_14
  *         @arg @ref LL_GPIO_AF_15
  * @retval None
  */
__STATIC_INLINE void LL_GPIO_SetAFPin_0_7(GPIO_TypeDef* GPIOx, uint32_t Pin, uint32_t Alternate)
{
  MODIFY_REG(GPIOx->AFR[0], (GPIO_AFRL_AFSEL0 << (POSITION_VAL(Pin) * 4)), (Alternate << (POSITION_VAL(Pin) * 4)));
}

/**
  * @brief  Return gpio alternate function for a dedicated pin from 0 to 7 
  *         included, of a dedicated port. 
  * @rmtoll AFRL         AFSELy        LL_GPIO_GetAFPin_0_7
  * @param  GPIOx GPIO Port
  * @param  Pin This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  * @retval Gpio pin alternate value
  */
__STATIC_INLINE uint32_t LL_GPIO_GetAFPin_0_7(GPIO_TypeDef* GPIOx, uint32_t Pin)
{
  return (uint32_t)(READ_BIT(GPIOx->AFR[0], (GPIO_AFRL_AFSEL0 << (POSITION_VAL(Pin) * 4))));
}

/**
  * @brief  Configure gpio alternate function for a dedicated pin from 8 to 15
  *         included, of a dedicated port. Possible values are from AF0 to
  *         AF15 depending on target.
  * @note   Warning: only one pin can be passed as parameter.
  * @rmtoll AFRH         AFSELy        LL_GPIO_SetAFPin_8_15
  * @param  GPIOx GPIO Port
  * @param  Pin This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  * @param  Alternate This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_AF_0
  *         @arg @ref LL_GPIO_AF_1
  *         @arg @ref LL_GPIO_AF_2
  *         @arg @ref LL_GPIO_AF_3
  *         @arg @ref LL_GPIO_AF_4
  *         @arg @ref LL_GPIO_AF_5
  *         @arg @ref LL_GPIO_AF_6
  *         @arg @ref LL_GPIO_AF_7
  *         @arg @ref LL_GPIO_AF_8
  *         @arg @ref LL_GPIO_AF_9
  *         @arg @ref LL_GPIO_AF_10
  *         @arg @ref LL_GPIO_AF_11
  *         @arg @ref LL_GPIO_AF_12
  *         @arg @ref LL_GPIO_AF_13
  *         @arg @ref LL_GPIO_AF_14
  *         @arg @ref LL_GPIO_AF_15
  * @retval None
  */
__STATIC_INLINE void LL_GPIO_SetAFPin_8_15(GPIO_TypeDef* GPIOx, uint32_t Pin, uint32_t Alternate)
{
  MODIFY_REG(GPIOx->AFR[1], (GPIO_AFRH_AFSEL8 << (POSITION_VAL(Pin >> 8) * 4)), (Alternate << (POSITION_VAL(Pin >> 8) * 4)));
}

/**
  * @brief  Return gpio alternate function for a dedicated pin from 8 to 15
  *         included, of a dedicated port. Possible values are from AF0 to
  *         AF15 depending on target.
  * @rmtoll AFRH         AFSELy        LL_GPIO_GetAFPin_8_15
  * @param  GPIOx GPIO Port
  * @param  Pin This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  * @retval Gpio pin alternate value
  */
__STATIC_INLINE uint32_t LL_GPIO_GetAFPin_8_15(GPIO_TypeDef* GPIOx, uint32_t Pin)
{
  return (uint32_t)(READ_BIT(GPIOx->AFR[1], (GPIO_AFRH_AFSEL8 << (POSITION_VAL(Pin >> 8) * 4))));
}

#if defined(ANALOG_SWITCH_CONTROL)
/**
  * @brief  Connect for several pins of a dedicated port, analog switch to ADC
  *         input.
  * @note   This bis must be set prior to the ADC conversion.
  *         Only the IO which connected to the ADC input are effective. 
  *         Other IO must be kept reset value
  * @rmtoll ASCR         ASCy          LL_GPIO_EnablePinAnalogControl
  * @param  GPIOx GPIO Port
  * @param  PinMask This parameter can be a combination of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  *         @arg @ref LL_GPIO_PIN_ALL
  * @retval None
  */
__STATIC_INLINE void LL_GPIO_EnablePinAnalogControl(GPIO_TypeDef* GPIOx, uint32_t PinMask)
{
  SET_BIT(GPIOx->ASCR, PinMask);
}

/**
  * @brief  Disconnect for several pins of a dedicated port, analog switch to ADC
  *         input.
  * @rmtoll ASCR         ASCy          LL_GPIO_DisablePinAnalogControl
  * @param  GPIOx GPIO Port
  * @param  PinMask This parameter can be a combination of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  *         @arg @ref LL_GPIO_PIN_ALL
  * @retval None
  */
__STATIC_INLINE void LL_GPIO_DisablePinAnalogControl(GPIO_TypeDef* GPIOx, uint32_t PinMask)
{
  CLEAR_BIT(GPIOx->ASCR, PinMask);
}
#endif /* ANALOG_SWITCH_CONTROL */

/**
  * @brief  Lock configuration of several pins of dedicated port. 
  *         When the lock sequence has been applied on a port bit, the
  *         value of this port bit can no longer be modified until the
  *         next reset.
  * @note   Each lock bit freezes a specific configuration register 
  *         (control and alternate function registers).
  * @rmtoll LCKR         LCKK          LL_GPIO_LockPin
  * @param  GPIOx GPIO Port
  * @param  PinMask This parameter can be a combination of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  *         @arg @ref LL_GPIO_PIN_ALL
  * @retval None
  */
__STATIC_INLINE void LL_GPIO_LockPin(GPIO_TypeDef* GPIOx, uint32_t PinMask)
{
  __IO uint32_t temp;
  WRITE_REG(GPIOx->LCKR, GPIO_LCKR_LCKK | PinMask);
  WRITE_REG(GPIOx->LCKR, PinMask);
  WRITE_REG(GPIOx->LCKR, GPIO_LCKR_LCKK | PinMask);
  temp = READ_REG(GPIOx->LCKR);
  (void) temp;
}

/**
  * @brief  Return 1 if all pins passed as parameter, for dedicated port, are 
  *         locked. Else Return 0.
  * @rmtoll LCKR         LCKy          LL_GPIO_IsPinLocked
  * @param  GPIOx GPIO Port
  * @param  PinMask This parameter can be a combination of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  *         @arg @ref LL_GPIO_PIN_ALL
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_GPIO_IsPinLocked(GPIO_TypeDef* GPIOx, uint32_t PinMask)
{
  return (READ_BIT(GPIOx->LCKR, PinMask) == (PinMask));
}

/**
  * @brief  Return 1 if one of the pin of a dedicated port is locked. Else return 
  *         0.
  * @rmtoll LCKR         LCKK          LL_GPIO_IsAnyPinLocked
  * @param  GPIOx GPIO Port
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_GPIO_IsAnyPinLocked(GPIO_TypeDef* GPIOx)
{
  return (READ_BIT(GPIOx->LCKR, GPIO_LCKR_LCKK) == (GPIO_LCKR_LCKK));
}

/**
  * @}
  */

/** @defgroup GPIO_LL_EF_Data_Access Data_Access
  * @{
  */

/**
  * @brief  Return full input data register value for a dedicated port.
  * @rmtoll IDR          IDy           LL_GPIO_ReadInputPort
  * @param  GPIOx GPIO Port
  * @retval Input data register value of port 
  */
__STATIC_INLINE uint32_t LL_GPIO_ReadInputPort(GPIO_TypeDef* GPIOx)
{
  return (uint32_t)(READ_REG(GPIOx->IDR));
}

/**
  * @brief  Return if input data level for several pins of dedicated port is high
  *         or low.
  * @rmtoll IDR          IDy           LL_GPIO_IsInputPinSet
  * @param  GPIOx GPIO Port
  * @param  PinMask This parameter can be a combination of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  *         @arg @ref LL_GPIO_PIN_ALL
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_GPIO_IsInputPinSet(GPIO_TypeDef* GPIOx, uint32_t PinMask)
{
  return (READ_BIT(GPIOx->IDR, PinMask) == (PinMask));
}

/**
  * @brief  Write output data register for the port.
  * @rmtoll ODR          ODy           LL_GPIO_WriteOutputPort
  * @param  GPIOx GPIO Port
  * @param  PortValue Level value for each pin of the port
  * @retval None
  */
__STATIC_INLINE void LL_GPIO_WriteOutputPort(GPIO_TypeDef* GPIOx, uint32_t PortValue)
{
  WRITE_REG(GPIOx->ODR, PortValue);
}

/**
  * @brief  Return full output data register value for a dedicated port.
  * @rmtoll ODR          ODy           LL_GPIO_ReadOutputPort
  * @param  GPIOx GPIO Port
  * @retval Output data register value of port
  */
__STATIC_INLINE uint32_t LL_GPIO_ReadOutputPort(GPIO_TypeDef* GPIOx)
{
  return (uint32_t)(READ_REG(GPIOx->ODR));
}

/**
  * @brief  Return if input data level for several pins of dedicated port is high 
  *         or low.
  * @rmtoll ODR          ODy           LL_GPIO_IsOutputPinSet
  * @param  GPIOx GPIO Port
  * @param  PinMask This parameter can be a combination of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  *         @arg @ref LL_GPIO_PIN_ALL
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_GPIO_IsOutputPinSet(GPIO_TypeDef* GPIOx, uint32_t PinMask)
{
  return (READ_BIT(GPIOx->ODR, PinMask) == (PinMask));
}

/**
  * @brief  Set several pins to high level on dedicated gpio port
  * @rmtoll BSRR         BSy           LL_GPIO_SetOutputPin
  * @param  GPIOx GPIO Port
  * @param  PinMask This parameter can be a combination of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  *         @arg @ref LL_GPIO_PIN_ALL
  * @retval None
  */
__STATIC_INLINE void LL_GPIO_SetOutputPin(GPIO_TypeDef* GPIOx, uint32_t PinMask)
{
  SET_BIT(GPIOx->BSRR, PinMask);
}

/**
  * @brief  Set several pins to low level on dedicated gpio port
  * @rmtoll BRR          BRy           LL_GPIO_ResetOutputPin
  * @param  GPIOx GPIO Port
  * @param  PinMask This parameter can be a combination of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  *         @arg @ref LL_GPIO_PIN_ALL
  * @retval None
  */
__STATIC_INLINE void LL_GPIO_ResetOutputPin(GPIO_TypeDef* GPIOx, uint32_t PinMask)
{
  SET_BIT(GPIOx->BRR, PinMask);
}

/**
  * @brief  Toggle data value for several pin of dedicated port
  * @rmtoll ODR          ODy           LL_GPIO_TogglePin
  * @param  GPIOx GPIO Port
  * @param  PinMask This parameter can be a combination of the following values:
  *         @arg @ref LL_GPIO_PIN_0
  *         @arg @ref LL_GPIO_PIN_1
  *         @arg @ref LL_GPIO_PIN_2
  *         @arg @ref LL_GPIO_PIN_3
  *         @arg @ref LL_GPIO_PIN_4
  *         @arg @ref LL_GPIO_PIN_5
  *         @arg @ref LL_GPIO_PIN_6
  *         @arg @ref LL_GPIO_PIN_7
  *         @arg @ref LL_GPIO_PIN_8
  *         @arg @ref LL_GPIO_PIN_9
  *         @arg @ref LL_GPIO_PIN_10
  *         @arg @ref LL_GPIO_PIN_11
  *         @arg @ref LL_GPIO_PIN_12
  *         @arg @ref LL_GPIO_PIN_13
  *         @arg @ref LL_GPIO_PIN_14
  *         @arg @ref LL_GPIO_PIN_15
  *         @arg @ref LL_GPIO_PIN_ALL
  * @retval None
  */
__STATIC_INLINE void LL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint32_t PinMask)
{
  WRITE_REG(GPIOx->ODR, READ_REG(GPIOx->ODR) ^ PinMask);
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

#endif /* defined (GPIOA) || defined (GPIOB) || defined (GPIOC) || defined (GPIOD) || defined (GPIOE) || defined (GPIOF) || defined (GPIOG) || defined (GPIOH) */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32L4xx_LL_GPIO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
