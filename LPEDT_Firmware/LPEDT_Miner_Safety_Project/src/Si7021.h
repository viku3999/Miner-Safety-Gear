/*
 * Si7021.h
 *
 *  Created on: Oct 10, 2024
 *      Author: vishn
 */

#ifndef SRC_SI7021_H_
#define SRC_SI7021_H_

#define EVENT_LETIMER_UF 0
#define EVENT_LETIMER_COMP1 1
#define EVENT_I2C_TRANSFER_COMPLETE 2
#define EVENT_NONE 3

#define PB0_BIT_POS 4
#define PB1_BIT_POS 5

/**
 * @brief   State machine to get the temperature from Si7021 chip over I2C using
 *          IRQs
 * @param   event -   event occurring outside of the state machine using which
 *                    the state machine shall determine the next course of action
 * @return  none
 */
void temperature_state_machine(uint32_t event);


#endif /* SRC_SI7021_H_ */
