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
 * @file    timers.h
 * @brief   Header file for timers.h Contains functions to use various onboard
 *          timers
 *
 * @author  Vishnu Kumar Thoodur Venkatachalapathy
 * @date    Jan 30, 2024
 */

#ifndef SRC_TIMERS_H_
#define SRC_TIMERS_H_

#define LETIMER_PERIOD_MS (3000)
#define LETIMER_ON_TIME_MS (175)

void LETIMER0_Set_Comp1(uint32_t load_value);

/**
 * @brief   Enables the LETIMER0 module based on the given energy mode
 * @param   nrg_mode   Energy mode in which the microcontroller is going to run
 *                     Accepted values: 0, 1, 2, 3
 * @return  none
 */
void LETIMER0_Enable(uint32_t nrg_mode);


/**
 * @brief   Provides a blocking delay of atleast us_wait micro-seconds based on
 *          the LETIMER0 ticks
 * @param   us_wait   Time to provide delay for in microseconds
 * @return  none
 */
void timerWaitUs_polled(uint32_t us_wait);


/**
 * @brief   Provides a non-blocking delay of atleast us_wait micro-seconds based on
 *          the LETIMER0 ticks
 * @param   us_wait   Time to provide delay for in microseconds
 * @return  none
 */
void timerWaitUs_irq(uint32_t us_wait);

#endif /* SRC_TIMERS_H_ */
