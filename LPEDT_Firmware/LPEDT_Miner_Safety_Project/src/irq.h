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
 * @brief   Header file for irq.c. Contains all Interrupt handelers for various peripherals
 *
 * @author  Vishnu Kumar Thoodur Venkatachalapathy
 * @date    Jan 30, 2024
 */

#ifndef SRC_IRQ_H_
#define SRC_IRQ_H_

/**
 * @brief IRQ handler for LETIMER0
 * @return  none
 */
void LETIMER0_IRQHandler(void);

/**
 * @brief   Function to calculate the amount of time passed since the system
 *          was powered on
 * @return  Time since system was powered on in milliseconds
 */
uint32_t letimerMilliseconds();

#endif /* SRC_IRQ_H_ */
