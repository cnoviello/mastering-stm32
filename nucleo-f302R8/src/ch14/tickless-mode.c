#include <limits.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f3xx_hal.h"

/* This is used to print message on the serial console. */
#define TICKLESS_DEBUG              1

/*-----------------------------------------------------------*/

/* Calculate how many clock increments make up a single tick period.
 Since we are using a prescaler equal to 1999, and assuming a clock
 speed of 64MHz, according the equation [1] in Chapter 11 this
 period value ensure a timer overflow ever 1ms. */
static const uint32_t ulMaximumPrescalerValue = 1999;
static const uint32_t ulPeriodValueForOneTick = 31;

/* Holds the maximum number of ticks that can be suppressed - which is
 basically how far into the future an interrupt can be generated without
 loosing the overflow event at all. It is set during initialization. */
static TickType_t xMaximumPossibleSuppressedTicks = 0;

/* Flag set from the tick interrupt to allow the sleep processing to know if
 sleep mode was exited because of an tick interrupt or a different interrupt. */
static volatile uint8_t ucTickFlag = pdFALSE;

/* The HAL handler of the TIM2 timer */
TIM_HandleTypeDef htim2;

#if defined(TICKLESS_DEBUG) && TICKLESS_DEBUG == 1

/* Used to store the debug message */
static char pcUARTMessage[100];

/* Flag set when the vPortSuppressTicksAndSleep() enters in SLEEP mode.
 It prevents that the UART console always prints the same message */
volatile uint8_t ucSleepModeCalled;

/* The handle to the UART peripheral used to print message on the console */
extern UART_HandleTypeDef huart2;

#endif /* #if defined(TICKLESS_DEBUG) && TICKLESS_DEBUG == 1 */

/*-----------------------------------------------------------*/

void xPortSysTickHandler( void );

/* The callback function called by the HAL when TIM2 overflows. */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM2) {
    xPortSysTickHandler();

    /* In case this is the first tick since the MCU left a low power mode.
     The period is so configured by vPortSuppressTicksAndSleep(). Here
     the reload value is reset to its default. */
    __HAL_TIM_SET_AUTORELOAD(htim, ulPeriodValueForOneTick);
    __HAL_TIM_SET_COUNTER(htim, 0);

    /* The CPU woke because of a tick. */
    ucTickFlag = pdTRUE;
  }
}
/*-----------------------------------------------------------*/

/* Override the default definition of vPortSetupTimerInterrupt() that is weakly
 defined in the FreeRTOS Cortex-M0 port layer with a version that configures TIM2
 to generate the tick interrupt. */
void vPortSetupTimerInterrupt(void) {
 
  /* Enable the TIM2 clock. */
  __HAL_RCC_TIM2_CLK_ENABLE();

  /* Ensure clock stops in debug mode. */
  __HAL_DBGMCU_FREEZE_TIM2();

  /* Configure the TIM2 timer */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = (uint16_t) ulMaximumPrescalerValue;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = ulPeriodValueForOneTick;
  HAL_TIM_Base_Init(&htim2);

  /* Enable the TIM2 interrupt. This must execute at the lowest interrupt priority. */
  HAL_NVIC_SetPriority(TIM2_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);

  HAL_TIM_Base_Start_IT(&htim2);
  /* See the comments where xMaximumPossibleSuppressedTicks is declared. */
  xMaximumPossibleSuppressedTicks = ((unsigned long) USHRT_MAX)
      / ulPeriodValueForOneTick;
}
/*-----------------------------------------------------------*/

/* Override the default definition of vPortSuppressTicksAndSleep() that is
 weakly defined in the FreeRTOS Cortex-M0 port layer with a version that manages
 the TIM2 interrupt, as the tick is generated from TIM2 compare matches events.

         THIS FUNCTION IS CALLED WITH THE SCHEDULER SUSPENDED.
 */
void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime) {
  uint32_t ulCounterValue, ulCompleteTickPeriods;
  eSleepModeStatus eSleepAction;
  TickType_t xModifiableIdleTime;
  const TickType_t xRegulatorOffIdleTime = 50;

  /* Make sure the TIM2 reload value does not overflow the counter. */
  if (xExpectedIdleTime > xMaximumPossibleSuppressedTicks) {
    xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
  }

  /* Calculate the reload value required to wait xExpectedIdleTime tick
   periods. */
  ulCounterValue = ulPeriodValueForOneTick * xExpectedIdleTime;

  /* To avoid race conditions, enter a critical section.  */
  __disable_irq();

  /* If a context switch is pending then abandon the low power entry as
   the context switch might have been pended by an external interrupt that
   requires processing. */
  eSleepAction = eTaskConfirmSleepModeStatus();
  if (eSleepAction == eAbortSleep) {
    /* Re-enable interrupts. */
    __enable_irq();
    return;
  } else if (eSleepAction == eNoTasksWaitingTimeout) {

#if defined(TICKLESS_DEBUG) && TICKLESS_DEBUG == 1
    if(ucSleepModeCalled == 0) { /* This ensures that the message is printed just once */
      ucSleepModeCalled = 1;
      sprintf(pcUARTMessage, "No more running tasks and delays: we can enter STOP mode\r\n");
      HAL_UART_Transmit(&huart2, (uint8_t*)pcUARTMessage, strlen(pcUARTMessage), HAL_MAX_DELAY);
    }
#endif

    /* Stop TIM2 */
    HAL_TIM_Base_Stop_IT(&htim2);

    /* A user definable macro that allows application code to be inserted
     here.  Such application code can be used to minimize power consumption
     further by turning off IO, peripheral clocks, the Flash, etc. */
    configPRE_STOP_PROCESSING();


    /* There are no running state tasks and no tasks that are blocked with a
     time out.  Assuming the application does not care if the tick time slips
     with respect to calendar time then enter a deep sleep that can only be
     woken by (in this demo case) the user button being pushed on the
     STM32L discovery board.  If the application does require the tick time
     to keep better track of the calendar time then the RTC peripheral can be
     used to make rough adjustments. */
    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);

    /* A user definable macro that allows application code to be inserted
     here.  Such application code can be used to reverse any actions taken
     by the configPRE_STOP_PROCESSING().  In this demo
     configPOST_STOP_PROCESSING() is used to re-initialize the clocks that
     were turned off when STOP mode was entered. */
    configPOST_STOP_PROCESSING();

#if defined(TICKLESS_DEBUG) && TICKLESS_DEBUG == 1
    if(ucSleepModeCalled == 1) { /* This ensures that the message is printed just once */
      ucSleepModeCalled = 0;
      sprintf(pcUARTMessage, "Exiting from STOP mode\r\n");
      HAL_UART_Transmit(&huart2, (uint8_t*)pcUARTMessage, strlen(pcUARTMessage), HAL_MAX_DELAY);
    }
#endif

    /* Restart tick. */
    HAL_TIM_Base_Start_IT(&htim2);

    /* Re-enable interrupts. */
    __enable_irq();
  } else {
    /* Stop TIM2 momentarily.  The time TIM2 is stopped for is not accounted for
     in this implementation (as it is in the generic implementation) because the
     clock is so slow it is unlikely to be stopped for a complete count period
     anyway. */
    HAL_TIM_Base_Stop_IT(&htim2);

    /* The tick flag is set to false before sleeping.  If it is true when sleep
     mode is exited then sleep mode was probably exited because the tick was
     suppressed for the entire xExpectedIdleTime period. */
    ucTickFlag = pdFALSE;

    /* Trap underflow before the next calculation. */
    configASSERT(ulCounterValue >= __HAL_TIM_GET_COUNTER(&htim2));

    /* Adjust the TIM2 value to take into account that the current time
     slice is already partially complete. */
    ulCounterValue -= (uint32_t) __HAL_TIM_GET_COUNTER(&htim2);

    /* Trap overflow/underflow before the calculated value is written to TIM2. */
    configASSERT(ulCounterValue < ( uint32_t ) USHRT_MAX);
    configASSERT(ulCounterValue != 0);

    /* Update to use the calculated overflow value. */
    __HAL_TIM_SET_AUTORELOAD(&htim2, ulCounterValue);
    __HAL_TIM_SET_COUNTER(&htim2, 0);

#if defined(TICKLESS_DEBUG) && TICKLESS_DEBUG == 1
    if(ucSleepModeCalled == 0) { /* This ensures that the message is printed just once */
      ucSleepModeCalled = 1;
      sprintf(pcUARTMessage, "eTaskConfirmSleepModeStatus returned eStandardSleep\r\n");
      HAL_UART_Transmit(&huart2, (uint8_t*)pcUARTMessage, strlen(pcUARTMessage), HAL_MAX_DELAY);

      sprintf(pcUARTMessage, "Expected idle time in ticks: %lu\r\n", xExpectedIdleTime);
      HAL_UART_Transmit(&huart2, (uint8_t*)pcUARTMessage, strlen(pcUARTMessage), HAL_MAX_DELAY);

      sprintf(pcUARTMessage, "MCU will sleep for %lu ticks\r\n", ulCounterValue/ulPeriodValueForOneTick);
      HAL_UART_Transmit(&huart2, (uint8_t*)pcUARTMessage, strlen(pcUARTMessage), HAL_MAX_DELAY);
    }
#endif

    /* Restart the TIM2. */
    HAL_TIM_Base_Start_IT(&htim2);

    /* Allow the application to define some pre-sleep processing.  This is
     the standard configPRE_SLEEP_PROCESSING() macro as described on the
     FreeRTOS.org website. */
    xModifiableIdleTime = xExpectedIdleTime;
    configPRE_SLEEP_PROCESSING( xModifiableIdleTime );

    /* xExpectedIdleTime being set to 0 by configPRE_SLEEP_PROCESSING()
     means the application defined code has already executed the wait/sleep
     instruction. */
    if (xModifiableIdleTime > 0) {
      /* The sleep mode used is dependent on the expected idle time
       as the deeper the sleep the longer the wake up time.  See the
       comments at the top of main_low_power.c.  Note xRegulatorOffIdleTime
       is set purely for convenience of demonstration and is not intended
       to be an optimized value. */
      if (xModifiableIdleTime > xRegulatorOffIdleTime) {
#if defined(TICKLESS_DEBUG) && TICKLESS_DEBUG == 1
    if(ucSleepModeCalled == 1) { /* This ensures that the message is printed just once */
      ucSleepModeCalled = 2;
      sprintf(pcUARTMessage, "There is sufficient time to enter a deeper SLEEP mode\r\n");
      HAL_UART_Transmit(&huart2, (uint8_t*)pcUARTMessage, strlen(pcUARTMessage), HAL_MAX_DELAY);
    }
#endif
        /* A slightly lower power sleep mode with a longer wake up time. */
        HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
      } else {
#if defined(TICKLESS_DEBUG) && TICKLESS_DEBUG == 1
    if(ucSleepModeCalled == 1) { /* This ensures that the message is printed just once */
      ucSleepModeCalled = 2;
      sprintf(pcUARTMessage, "There is no sufficient time to enter a deeper SLEEP mode\r\n");
      HAL_UART_Transmit(&huart2, (uint8_t*)pcUARTMessage, strlen(pcUARTMessage), HAL_MAX_DELAY);
    }
#endif
        /* A slightly higher power sleep mode with a faster wake up time. */
        HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
      }
    }

    /* Allow the application to define some post sleep processing.  This is
     the standard configPOST_SLEEP_PROCESSING() macro, as described on the
     FreeRTOS.org website. */
    configPOST_SLEEP_PROCESSING( xModifiableIdleTime );

    /* Re-enable interrupts. If the timer has overflowed during this period
     then this will cause that the TIM2_IRQHandler() is called. So the
     global tick counter is incremented by 1 and the ulTickFlag variable
     is set to pdTRUE.
     Take note that in the STM32L example in the official FreeRTOS
     distribution interrupts are re-enabled after the TIM2 is stopped.
     This is wrong, because it causes that the IRQ is leaved pending,
     even if has been set. So we must first re-enable interrupts - this
     causes that a pending TIM2 IRQ fires - and then stop the timer. */
    __enable_irq();

    /* Stop TIM2.  Again, the time the clock is stopped for in not accounted
     for here (as it would normally be) because the clock is so slow it is
     unlikely it will be stopped for a complete count period anyway. */
    HAL_TIM_Base_Stop_IT(&htim2);

    if (ucTickFlag != pdFALSE) {

#if defined(TICKLESS_DEBUG) && TICKLESS_DEBUG == 1
      if(ucSleepModeCalled == 2) { /* This ensures that the message is printed just once */
        ucSleepModeCalled = 3;
        sprintf(pcUARTMessage, "TIM2 woken up the MCU and we can increase the tick count with %lu ticks\r\n", xExpectedIdleTime -1UL);
        HAL_UART_Transmit(&huart2, (uint8_t*)pcUARTMessage, strlen(pcUARTMessage), HAL_MAX_DELAY);
      }
#endif
      /* The MCU has been woken up by the TIM2. So we trap overflows
       before the next calculation. */
      configASSERT(
          ulPeriodValueForOneTick >= (uint32_t ) __HAL_TIM_GET_COUNTER(&htim2));

      /* The tick interrupt has already executed, although because this
       function is called with the scheduler suspended the actual tick
       processing will not occur until after this function has exited.
       Reset the reload value with whatever remains of this tick period. */
      ulCounterValue = ulPeriodValueForOneTick
          - (uint32_t) __HAL_TIM_GET_COUNTER(&htim2);

      /* Trap under/overflows before the calculated value is used. */
      configASSERT(ulCounterValue <= ( uint32_t ) USHRT_MAX);
      configASSERT(ulCounterValue != 0);

      /* Use the calculated reload value. */
      __HAL_TIM_SET_AUTORELOAD(&htim2, ulCounterValue);
      __HAL_TIM_SET_COUNTER(&htim2, 0);

      /* The tick interrupt handler will already have pended the tick
       processing in the kernel.  As the pending tick will be processed as
       soon as this function exits, the tick value	maintained by the tick
       is stepped forward by one less than the	time spent sleeping.  The
       actual stepping of the tick appears later in this function. */
      ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
    } else {
      /* Something other than the tick interrupt ended the sleep.  How
       many complete tick periods passed while the processor was
       sleeping? */
      ulCompleteTickPeriods = ((uint32_t) __HAL_TIM_GET_COUNTER(&htim2))
          / ulPeriodValueForOneTick;

      /* Check for over/under flows before the following calculation. */
      configASSERT(
          ((uint32_t ) __HAL_TIM_GET_COUNTER(&htim2)) >= (ulCompleteTickPeriods * ulPeriodValueForOneTick));

      /* The reload value is set to whatever fraction of a single tick
       period remains. */
      ulCounterValue = ((uint32_t) __HAL_TIM_GET_COUNTER(&htim2))
          - (ulCompleteTickPeriods * ulPeriodValueForOneTick);
      configASSERT(ulCounterValue <= ( uint32_t ) USHRT_MAX);
      if (ulCounterValue == 0) {
        /* There is no fraction remaining. */
        ulCounterValue = ulPeriodValueForOneTick;
        ulCompleteTickPeriods++;
      }
      __HAL_TIM_SET_AUTORELOAD(&htim2, ulCounterValue);
      __HAL_TIM_SET_COUNTER(&htim2, 0);

#if defined(TICKLESS_DEBUG) && TICKLESS_DEBUG == 1
      if(ucSleepModeCalled == 2) { /* This ensures that the message is printed just once */
        ucSleepModeCalled = 3;
        sprintf(pcUARTMessage, "Another IRQ woken up the MCU and we can increase the tick count with %lu ticks\r\n", ulCompleteTickPeriods);
        HAL_UART_Transmit(&huart2, (uint8_t*)pcUARTMessage, strlen(pcUARTMessage), HAL_MAX_DELAY);
      }
#endif
    }

    /* Restart TIM2 so it runs up to the reload value.  The reload value
     will get set to the value required to generate exactly one tick period
     the next time the TIM2 interrupt executes. */
    HAL_TIM_Base_Start_IT(&htim2);

    /* Wind the tick forward by the number of tick periods that the CPU
     remained in a low power state. */
    vTaskStepTick(ulCompleteTickPeriods);
  }
}
