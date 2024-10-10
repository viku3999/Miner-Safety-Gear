/*******************************************************************************
 * Copyright (C) 2024 by Vishnu Kumar Thoodur Venkatachalapathy
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted as long as the files maintain this copyright. Users are
 * permitted to modify this and use it to learn about the field of embedded
 * software. Vishnu Kumar Thoodur Venkatachalapathy and the University of
 * Colorado are not liable for any misuse of this material.
 * ****************************************************************************/

#define EM0 0
#define EM1 1
#define EM2 2
#define EM3 3

/**
 * @file    oscillators.h
 * @brief   Header file for oscillators.c. Constains functions to setup clocks
 *          for various peripherals
 *
 * @author  Vishnu Kumar Thoodur Venkatachalapathy
 * @date    Jan 30, 2024
 */
#ifndef SRC_OSCILLATORS_H_
#define SRC_OSCILLATORS_H_

/**
 * @brief Setups the required clocks for the LETIMER0 module based on given
 *        energy modes and prescaler value
 * @param   nrg_mode   Energy mode in which the microcontroller is going to run
 *                     Accepted values: 0, 1, 2, 3
 * @param    prescaler  prescaler value to load to the of the LETIMER0 module clk
 * @return  none
 */
void LETIMER0_clk_Enable(uint32_t nrg_mode, uint32_t prescaler);

#endif /* SRC_OSCILLATORS_H_ */
