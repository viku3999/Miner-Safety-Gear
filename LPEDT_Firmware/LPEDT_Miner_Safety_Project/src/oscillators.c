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
 * @file    oscillators.c
 * @brief   Functions to setup clocks for various peripherals
 *
 * @author  Vishnu Kumar Thoodur Venkatachalapathy
 * @date    Jan 30, 2024
 */

#include <stdbool.h>

#include "em_device.h"
#include "em_letimer.h"
#include "em_cmu.h"

/**
 * @brief Setups the required clocks for the LETIMER0 module based on given
 *        energy modes and prescaler value
 * @param   nrg_mode   Energy mode in which the microcontroller is going to run
 *                     Accepted values: 0, 1, 2, 3
 * @param    prescaler  prescaler value to load to the of the LETIMER0 module clk
 * @return  none
 */
void LETIMER0_clk_Enable(uint32_t nrg_mode, uint32_t prescaler){
  if(nrg_mode != 3){
    CMU_OscillatorEnable (cmuOsc_LFXO,true, true);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  }
  else{
    CMU_OscillatorEnable (cmuOsc_ULFRCO,true, true);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
  }
    CMU_ClockDivSet(cmuClock_LETIMER0,prescaler);
    CMU_ClockEnable(cmuClock_LETIMER0, true);
}
