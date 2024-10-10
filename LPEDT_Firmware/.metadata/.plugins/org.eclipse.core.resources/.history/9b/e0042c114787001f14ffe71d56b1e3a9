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
 * @file    scheduler.h
 * @brief   Header file for implementation of scheduler
 *
 * @author  Vishnu Kumar Thoodur Venkatachalapathy
 * @date    Feb 7, 2024
 */

#ifndef SRC_SCHEDULER_H_
#define SRC_SCHEDULER_H_

#define EVENT_LETIMER_UF 0
#define EVENT_LETIMER_COMP1 1
#define EVENT_I2C_TRANSFER_COMPLETE 2
#define EVENT_NONE 3

#define PB0_BIT_POS 4
#define PB1_BIT_POS 5

#include "ble.h"

/**
 * @brief   Scheduler to set the <>
 * @return  none
 */
void schedulerSetEventPB1();

/**
 * @brief   Scheduler to set the <>
 * @return  none
 */
void schedulerSetEventPB0();

/**
 * @brief   Scheduler to set the LETIMER0 event where COMP1 condition is met
 * @return  none
 */
void schedulerSetEventLETIMER0Comp1();

/**
 * @brief   Scheduler to set the LETIMER0 event where UF condition is met
 * @return  none
 */
void schedulerSetEventLETIMER0UF();


/**
 * @brief   Scheduler to set the event where I2C transfer is done
 * @return  none
 */
void schedulerSetEventI2CTransferDone();

/**
 * @brief   Checks if any event are present to handle and returns them. In case
 *          of multiple events, the most important event shall be handled first.
 * @return  Returns the latest event to handle
 */
uint32_t getNextEvent();

/**
 * @brief   State machine to get the temperature from Si7021 chip over I2C using
 *          IRQs
 * @param   event -   event occurring outside of the state machine using which
 *                    the state machine shall determine the next course of action
 * @return  none
 */
void temperature_state_machine(uint32_t event);

#if BUILD_INCLUDES_BLE_SERVER == 1
/**
 * @brief   State machine to get the temperature from Si7021 chip over I2C using
 *          IRQs and send it via BT
 * @param   event -   event occurring outside of the state machine using which
 *                    the state machine shall determine the next course of action
 * @return  none
 */
void temperature_state_machine_bt(sl_bt_msg_t *evt);
#endif

#if BUILD_INCLUDES_BLE_CLIENT == 1

void Discovery_State_Machine(sl_bt_msg_t *evt);
#endif

#endif /* SRC_SCHEDULER_H_ */
