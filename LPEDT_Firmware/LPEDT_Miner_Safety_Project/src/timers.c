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
 * @file    timers.c
 * @brief   Functions to use various onboard timers
 *
 * @author  Vishnu Kumar Thoodur Venkatachalapathy
 * @date    Jan 30, 2024
 */

#include <stdbool.h>

#include "em_device.h"
#include "oscillators.h"
#include "em_letimer.h"
#include "timers.h"
#include "em_cmu.h"

// Include logging specifically for this .c file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

#define LFXO_PRESCALER_VALUE (1)
#define ULFRCO_PRESCALER_VALUE (1)
#define LFXO_CLK_FREQ (32768/LFXO_PRESCALER_VALUE)
#define ULFRCO_CLK_FREQ (1000/ULFRCO_PRESCALER_VALUE)

#define COMP0_LOAD_VAL_EM0 ((LETIMER_PERIOD_MS*LFXO_CLK_FREQ)/(1000))
#define COMP0_LOAD_VAL_EM1 ((LETIMER_PERIOD_MS*LFXO_CLK_FREQ)/(1000))
#define COMP0_LOAD_VAL_EM2 ((LETIMER_PERIOD_MS*LFXO_CLK_FREQ)/(1000))
#define COMP0_LOAD_VAL_EM3 (((LETIMER_PERIOD_MS*ULFRCO_CLK_FREQ)/(1000)))

#define COMP1_LOAD_VAL_EM0 ((LETIMER_ON_TIME_MS*LFXO_CLK_FREQ)/(1000))
#define COMP1_LOAD_VAL_EM1 ((LETIMER_ON_TIME_MS*LFXO_CLK_FREQ)/(1000))
#define COMP1_LOAD_VAL_EM2 ((LETIMER_ON_TIME_MS*LFXO_CLK_FREQ)/(1000))
#define COMP1_LOAD_VAL_EM3 (((LETIMER_ON_TIME_MS*ULFRCO_CLK_FREQ)/(1000)))

uint32_t LETIMER0_Comp0_Load_Val = 0, LETIMER0_Comp1_Load_Val = 0;


/**
 * @brief   Sets the comp1 value in the LETIMER0 module
 * @param   load_value the value to load to the COMP1 register
 * @return  none
 */
void LETIMER0_Set_Comp1(uint32_t load_value){
  LETIMER0_Comp1_Load_Val = load_value;
  LETIMER_CompareSet(LETIMER0, 1, load_value);
}

/**
 * @brief   Enables the LETIMER0 module based on the given energy mode
 * @param   nrg_mode   Energy mode in which the microcontroller is going to run
 *                     Accepted values: 0, 1, 2, 3
 * @return  none
 */
void LETIMER0_Enable(uint32_t nrg_mode){
  uint32_t temp;

  LETIMER_Init_TypeDef letimerInitData = {
    false, // enable; don't enable when init completes, we'll enable last
    true, // debugRun; useful to have the timer running when single-stepping in the debugger
    true, // comp0Top; load COMP0 into CNT on underflow
    false, // bufTop; don't load COMP1 into COMP0 when REP0==0
    0, // out0Pol; 0 default output pin value
    0, // out1Pol; 0 default output pin value
    letimerUFOANone, // ufoa0; no underflow output action
    letimerUFOANone, // ufoa1; no underflow output action
    letimerRepeatFree, // repMode; free running mode i.e. load & go forever
    0 // COMP0(top) Value, I calculate this below
  };

  // Choose the clock for LETIMER0 based on selected energy mode
  if(nrg_mode == EM3){
      LETIMER0_clk_Enable(nrg_mode,ULFRCO_PRESCALER_VALUE);
  }
  else{
      LETIMER0_clk_Enable(nrg_mode,LFXO_PRESCALER_VALUE);
  }

  // init the timer
  LETIMER_Init (LETIMER0, &letimerInitData);

  // Set the comp0 and comp1 values based on energy mode
  switch(nrg_mode){
        case EM0: LETIMER0_Comp0_Load_Val = COMP0_LOAD_VAL_EM0;
                  LETIMER0_Comp1_Load_Val = COMP1_LOAD_VAL_EM0;
                  break;
        case EM1: LETIMER0_Comp0_Load_Val = COMP0_LOAD_VAL_EM1;
                  LETIMER0_Comp1_Load_Val = COMP1_LOAD_VAL_EM1;
                  break;
        case EM2: LETIMER0_Comp0_Load_Val = COMP0_LOAD_VAL_EM2;
                  LETIMER0_Comp1_Load_Val = COMP1_LOAD_VAL_EM2;
                  break;
        case EM3: LETIMER0_Comp0_Load_Val = COMP0_LOAD_VAL_EM3;
                  LETIMER0_Comp1_Load_Val = COMP1_LOAD_VAL_EM3;
                  break;
        default:  LETIMER0_Comp0_Load_Val = COMP0_LOAD_VAL_EM0;
                  LETIMER0_Comp1_Load_Val = COMP1_LOAD_VAL_EM0;
                  break;
      }

  LETIMER_CompareSet(LETIMER0, 0, LETIMER0_Comp0_Load_Val);
//  LETIMER0_Set_Comp1(LETIMER0_Comp1_Load_Val);

  // Setup Interrupts
  LETIMER_IntClear (LETIMER0, 0xFFFFFFFF); // punch them all down
  // Set UF and COMP1 in LETIMER0_IEN, so that the timer will generate IRQs to the NVIC.
  temp = LETIMER_IEN_UF | LETIMER_IEN_COMP1;
  LETIMER_IntEnable (LETIMER0, temp); // Make sure you have defined the ISR routine LETIMER0_IRQHandler()
  NVIC_ClearPendingIRQ (LETIMER0_IRQn);
  NVIC_EnableIRQ(LETIMER0_IRQn);

  // Enable the timer to starting counting down, set LETIMER0_CMD[START] bit, see LETIMER0_STATUS[RUNNING] bit
  LETIMER_Enable (LETIMER0, true);
}

/**
 * @brief   Provides a blocking delay of atleast us_wait micro-seconds based on
 *          the LETIMER0 ticks
 * @param   us_wait   Time to provide delay for in microseconds
 * @return  none
 */
void timerWaitUs_polled(uint32_t us_wait){
  uint32_t LETIMER_TICK_1 = 0, LETIMER_TICK_2 = 0;

  // Calculate how long each tick is based on the clock frequency
  // in micro-seconds
  uint32_t LETIMER_Freq = CMU_ClockFreqGet(cmuClock_LETIMER0);
  uint32_t LETIMER_Tick_Time_us = 1000000/LETIMER_Freq;

  // Range checking the input. If requested time is less than what this function
  // can provide, then we provide a minimum time delay given supported by 1 tick
  // If the requested time is more that the max time supported by the current
  // LETIMER0 configuration, then we provide maximum time supported by this
  // LETIMER0 configuration.
  if (us_wait < LETIMER_Tick_Time_us){
      us_wait = LETIMER_Tick_Time_us;
      LOG_ERROR("Requested delay is below the supported range. Replacing with delay of %dus", us_wait);
  }

  if( us_wait > (LETIMER_Tick_Time_us * LETIMER0_Comp0_Load_Val)){
      us_wait = LETIMER_Tick_Time_us  * LETIMER0_Comp0_Load_Val;
      LOG_ERROR("Requested delay is more than the supported range. Replacing with delay of %dus", us_wait);
  }

  // Calculating the number of ticks required to achieve the desired time
  uint32_t LETIMER_Tick_Count = us_wait/LETIMER_Tick_Time_us;

  // If the requested time is not completely divisible by the time taken for a
  // single LETIMER0 tick, then we count one tick extra so that we can meet the
  // given minimum requirement
  if((us_wait % LETIMER_Tick_Time_us) != 0)
    LETIMER_Tick_Count++;

  // Get the current non-zero tick count
  do{
      LETIMER_TICK_1 = LETIMER_CounterGet(LETIMER0);
  }while(LETIMER_TICK_1 == 0);

  // Wait till the required number of ticks are over
  while(true){
      LETIMER_TICK_2 = LETIMER_CounterGet(LETIMER0);
      if((LETIMER_TICK_1 - LETIMER_TICK_2) >= LETIMER_Tick_Count){
          break;
      }
  }
}

/**
 * @brief   Provides a non-blocking delay of atleast us_wait micro-seconds based on
 *          the LETIMER0 ticks
 * @param   us_wait   Time to provide delay for in microseconds
 * @return  none
 */
void timerWaitUs_irq(uint32_t us_wait){
  uint32_t LETIMER_Curr_Tick = 0, LETIMER_Comp1_New_Load_Val = 0;

  // Calculate how long each tick is based on the clock frequency
  // in micro-seconds
  uint32_t LETIMER_Tick_Time_us = 1000000/CMU_ClockFreqGet(cmuClock_LETIMER0);

  // Range checking the input. If requested time is less than what this function
  // can provide, then we provide a minimum time delay given supported by 1 tick
  // If the requested time is more that the max time supported by the current
  // LETIMER0 configuration, then we provide maximum time supported by this
  // LETIMER0 configuration.
  if (us_wait < LETIMER_Tick_Time_us){
      us_wait = LETIMER_Tick_Time_us;
      LOG_ERROR("Requested delay is below the supported range. Replacing with delay of %dus", us_wait);
  }

  if( us_wait > (LETIMER_Tick_Time_us * LETIMER0_Comp0_Load_Val)){
      us_wait = LETIMER_Tick_Time_us  * LETIMER0_Comp0_Load_Val;
      LOG_ERROR("Requested delay is more than the supported range. Replacing with delay of %dus", us_wait);
  }

  // Calculating the number of ticks required to achieve the desired time
  uint32_t LETIMER_Req_Tick_Count = us_wait/LETIMER_Tick_Time_us;

  // If the requested time is not completely divisible by the time taken for a
  // single LETIMER0 tick, then we count one tick extra so that we can meet the
  // given minimum requirement
  if((us_wait % LETIMER_Tick_Time_us) != 0)
    LETIMER_Req_Tick_Count++;

  // Get the current tick count
  LETIMER_Curr_Tick = LETIMER_CounterGet(LETIMER0);

  /*
   * Calculate the Comp1 load value and load it to LETIMER0. If the required
   * tick count is less than the current tick count, then we calculate the
   * comp1 load value by wrapping it to the top of the counter when the LETIMER
   * module restarts
   */
  if(LETIMER_Req_Tick_Count < LETIMER_Curr_Tick)
    LETIMER_Comp1_New_Load_Val = LETIMER_Curr_Tick - LETIMER_Req_Tick_Count;
  else
    LETIMER_Comp1_New_Load_Val = LETIMER0_Comp0_Load_Val - (LETIMER_Req_Tick_Count - LETIMER_Curr_Tick);

//  LETIMER_IntClear(LETIMER0, 0xFFFFFFFF);
  LETIMER0_Set_Comp1(LETIMER_Comp1_New_Load_Val);
//  LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP1);
//  LETIMER_TypeDef *letimer;
//  letimer = LETIMER0;
//  letimer->IEN |= LETIMER_IEN_COMP1;
}
