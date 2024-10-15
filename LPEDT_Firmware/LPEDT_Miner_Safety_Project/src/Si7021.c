/*
 * Si7021.c
 *
 *  Created on: Oct 10, 2024
 *      Author: vishn
 */

#include <em_core.h>
#include <sl_power_manager.h>
#include "src/scheduler.h"
#include "src/gpio.h"
#include "src/timers.h"
#include "i2c.h"

// Include logging specifically for this .c file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

#define NUM_STATES 5

// enum declarations used for temperature state machines
typedef enum uint32_t {
  stateIdle,
  waitForSi7021POR,
  waitForI2CWriteTransfer,
  waitForSi7021Conversion,
  waitForI2CReadTransfer
} State_t;


/**
 * @brief   State machine to get the temperature from Si7021 chip over I2C using
 *          IRQs
 * @param   event -   event occurring outside of the state machine using which
 *                    the state machine shall determine the next course of action
 * @return  none
 */
void temperature_state_machine(uint32_t event){
  State_t currentState;
  static State_t nextState = stateIdle;
  uint32_t temperature_reading = 0;
  uint16_t Si7021_data = 0;

  currentState = nextState;

  switch (currentState) {
    case stateIdle:
            nextState = stateIdle; // default
            /*
             * if event is LETIMERUF, then power on the Si7021, set delay for
             * Si7021 POR and go to next state
             */
              if (event == EVENT_LETIMER_UF) {
//                  si7021TurnOn();
                  BMI270_Get_Chip_Id();
                  BME688_Get_Chip_Id();
                  timerWaitUs_irq(SI7021_POR_TIME_US);
                  nextState = waitForSi7021POR;
              }
            break;
    case waitForSi7021POR:
            nextState = waitForSi7021POR; // default
            /*
             * if timer event is complete (denoted by the LETIMER_COMP1 event),
             * then set sleep to EM1, start I2C write operation to request
             * temperature data from the Si7021 chip and go to next state.
             */
              if (event == EVENT_LETIMER_COMP1) {
                  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1); // Setting sleep to EM1
                  I2C_Write_Data_itr(SI7021_DEVICE_ADDR, SI7021_CMD_MEASURE_TEMP_NO_HOLD);
                  nextState = waitForI2CWriteTransfer;
              }
            break;
    case waitForI2CWriteTransfer:
            nextState = waitForI2CWriteTransfer; // default
            /*
             * if i2c transfer is done, disable I2C IRQ, remove EM1 power
             * requirement, setup conversion timer required by the Si7021 chip
             * and go to next state.
             */
              if (event == EVENT_I2C_TRANSFER_COMPLETE) {
                  NVIC_DisableIRQ(I2C0_IRQn);
                  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1); // Setting sleep to EM3
                  timerWaitUs_irq(SI7021_14B_CONVERSION_TIME_US);
                  nextState = waitForSi7021Conversion;
              }
            break;
    case waitForSi7021Conversion:
            nextState = waitForSi7021Conversion; // default
            /*
            * if timer event is complete (denoted by the LETIMER_COMP1 event),
            * then set sleep to EM1, start I2C read operation to read the
            * requested temperature data from the Si7021 chip and go to next
            * state.
            */
              if (event == EVENT_LETIMER_COMP1) {
                  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1); // Setting sleep to EM1
                  I2C_Read_Data_irq(SI7021_DEVICE_ADDR);
                  nextState = waitForI2CReadTransfer;
              }
            break;
    case waitForI2CReadTransfer:
            // if i2c transfer is done, power-off the module, set sleep to EM3, retrive and print temp and go to next state
            nextState = waitForI2CReadTransfer; // default
            /*
             * if i2c transfer is done, disable I2C IRQ, remove EM1 power
             * requirement, retrive data from I2C read operation and print the temperature to the LOG console,
             * and go to next state.
             */
              if (event == EVENT_I2C_TRANSFER_COMPLETE) {
                  NVIC_DisableIRQ(I2C0_IRQn);
                  si7021TurnOff();
                  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1); // Setting sleep to EM3
                  Si7021_data = I2C_Get_Data();

                  // Converting the data received from the sensor into temperature in Celsius
                  // using the formula given in the SI7021 sensor application note AN607
                  temperature_reading = ((Si7021_data*175.72)/65536) - 46.85;
                  LOG_INFO("Temp1= %d°C\r\n\n", temperature_reading);
//                  BMI270_Get_Chip_Id();
                  nextState = stateIdle;
              }
            break;
  default:
            break;
  } // switch
} // state_machine()
