/*******************************************************************************
 * Copyright (C) 2024 by Vishnu Kumar Thoodur Venkatachalapathy
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted as long as the files maintain this copyright. Users are
 * permitted to modify this and use it to learn about the field of embedded
 * software. Vishnu Kumar Thoodur Venkatachalapathy and the University of
 * Colorado are not liable for any misuse of this material.
 * ****************************************************************************/

/**
 * @file    irq.c
 * @brief   Contains all Interrupt handelers for various peripherals
 *
 * @author  Vishnu Kumar Thoodur Venkatachalapathy
 * @date    Jan 30, 2024
 */

#include <stdio.h>
#include <em_cmu.h>

#include "em_letimer.h"
#include "gpio.h"
#include "scheduler.h"
#include "sl_i2cspm.h"
#include "timers.h"

#define LETIMER0_COMP1_FLAG 0x2
#define LETIMER0_UF_FLAG 0x4

#define LETIMER0_COMP1_FLAG_BIT_POS (1)
#define LETIMER0_UF_FLAG_BIT_POS (2)

#define PB0_FLAG_BIT_POS (6)
#define PB1_FLAG_BIT_POS (7)

// Include logging specifically for this .c file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

uint32_t rollover_count = 0;
I2C_TransferReturn_TypeDef IRQtransferStatus; // make this global for IRQs in A4

/**
 * @brief IRQ handler for LETIMER0
 * @return  none
 */
void LETIMER0_IRQHandler(void){

  // Get IRQ source
  uint32_t flags=0;
  flags = LETIMER_IntGetEnabled(LETIMER0);

  // Clear IRQ
  LETIMER_IntClear(LETIMER0, flags);

  // Servicing IRQ

//   Case where UF
  if(flags & (1<<LETIMER0_UF_FLAG_BIT_POS)){
      schedulerSetEventLETIMER0UF();
      rollover_count += 1;
  }

//  Case where COMP1 is true
  if(flags & (1<<LETIMER0_COMP1_FLAG_BIT_POS)){
      schedulerSetEventLETIMER0Comp1();
  }
}

/**
 * @brief IRQ handler for LETIMER0
 * @return  none
 */
void I2C0_IRQHandler(void){


  // Get IRQ source
//  uint32_t flags=0;
//  flags = LETIMER_IntGetEnabled(LETIMER0);

  // I2C transfer is executed until the transfer is done
  IRQtransferStatus = I2C_Transfer(I2C0);
  if (IRQtransferStatus == i2cTransferDone) {
      schedulerSetEventI2CTransferDone();
  }
  if (IRQtransferStatus < 0) {
      LOG_ERROR("%d", IRQtransferStatus);
  }
}

/**
 * @brief IRQ handler for GPIO even-numbered pins
 * @return  none
 */
void GPIO_EVEN_IRQHandler(void)
{
  // Get IRQ source
  uint32_t flags=0;
  flags = GPIO_IntGetEnabled();

  // Clear interrupt flags
  GPIO_IntClear(flags);

  if(flags & (1<<PB0_FLAG_BIT_POS)){
      schedulerSetEventPB0();
  }
}

/**
 * @brief IRQ handler for GPIO odd-numbered pins
 * @return  none
 */
void GPIO_ODD_IRQHandler(void)
{
  // Get IRQ source
   uint32_t flags=0;
   flags = GPIO_IntGetEnabled();

   // Clear interrupt flags
   GPIO_IntClear(flags);

   if(flags & (1<<PB1_FLAG_BIT_POS)){
       schedulerSetEventPB1();
   }
}


/**
 * @brief   Function to calculate the amount of time passed since the system
 *          was powered on
 * @return  Time since system was powered on in milliseconds
 */
uint32_t letimerMilliseconds(){
  // Calculate how long each tick is based on the clock frequency
  // in micro-seconds
  uint32_t LETIMER_Tick_Time_ms = 1000/CMU_ClockFreqGet(cmuClock_LETIMER0);

  // Get the current number of ticks passed after LETIMER counter reload
  uint32_t ctr = LETIMER_CompareGet(LETIMER0,0)-LETIMER_CounterGet(LETIMER0);

  uint32_t rollover_time_ms = rollover_count*LETIMER_PERIOD_MS;
  uint32_t curr_time_ms = LETIMER_Tick_Time_ms*ctr;
  uint32_t return_time_ms = curr_time_ms + rollover_time_ms;

  return (return_time_ms);
}
