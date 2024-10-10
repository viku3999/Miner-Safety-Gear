/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "em_common.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "app.h"

#include "sl_status.h"

#include "src/ble_device_type.h"
#include "src/gpio.h"
#include "src/lcd.h"

#include "src/timers.h"
#include "src/scheduler.h"
#include "src/i2c.h"

#include "src/ble.h"

// Include logging specifically for this .c file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

#define EM0 0
#define EM1 1
#define EM2 2
#define EM3 3

// *************************************************
// Power Manager
// *************************************************

// See: https://docs.silabs.com/gecko-platform/latest/service/power_manager/overview
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)

// -----------------------------------------------------------------------------
// defines for power manager callbacks
// -----------------------------------------------------------------------------
// Return values for app_is_ok_to_sleep():
//   Return false to keep sl_power_manager_sleep() from sleeping the MCU.
//   Return true to allow system to sleep when you expect/want an IRQ to wake
//   up the MCU from the call to sl_power_manager_sleep() in the main while (1)
//   loop.
//
// Students: We'll need to modify this for A2 onward so that compile time we
//           control what the lowest EM (energy mode) the MCU sleeps to. So
//           think "#if (expression)".
#if (LOWEST_ENERGY_MODE == 0)
#define APP_IS_OK_TO_SLEEP      (false)
#else
#define APP_IS_OK_TO_SLEEP      (true)
#endif

// Return values for app_sleep_on_isr_exit():
//   SL_POWER_MANAGER_IGNORE; // The module did not trigger an ISR and it doesn't want to contribute to the decision
//   SL_POWER_MANAGER_SLEEP;  // The module was the one that caused the system wakeup and the system SHOULD go back to sleep
//   SL_POWER_MANAGER_WAKEUP; // The module was the one that caused the system wakeup and the system MUST NOT go back to sleep
//
// Notes:
//       SL_POWER_MANAGER_IGNORE, we see calls to app_process_action() on each IRQ. This is the
//       expected "normal" behavior.
//
//       SL_POWER_MANAGER_SLEEP, the function app_process_action()
//       in the main while(1) loop will not be called! It would seem that sl_power_manager_sleep()
//       does not return in this case.
//
//       SL_POWER_MANAGER_WAKEUP, doesn't seem to allow ISRs to run. Main while loop is
//       running continuously, flooding the VCOM port with printf text with LETIMER0 IRQs
//       disabled somehow, LED0 is not flashing.

#define APP_SLEEP_ON_ISR_EXIT   (SL_POWER_MANAGER_IGNORE)
//#define APP_SLEEP_ON_ISR_EXIT   (SL_POWER_MANAGER_SLEEP)
//#define APP_SLEEP_ON_ISR_EXIT   (SL_POWER_MANAGER_WAKEUP)

#endif // defined(SL_CATALOG_POWER_MANAGER_PRESENT)

// *************************************************
// Power Manager Callbacks
// The values returned by these 2 functions AND
// adding and removing power manage requirements is
// how we control when EM mode the MCU goes to when
// sl_power_manager_sleep() is called in the main
// while (1) loop.
// *************************************************

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)

bool app_is_ok_to_sleep(void)
{
  return APP_IS_OK_TO_SLEEP;
} // app_is_ok_to_sleep()

sl_power_manager_on_isr_exit_t app_sleep_on_isr_exit(void)
{
  return APP_SLEEP_ON_ISR_EXIT;
} // app_sleep_on_isr_exit()

#endif // defined(SL_CATALOG_POWER_MANAGER_PRESENT)

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/

SL_WEAK void app_init(void)
{
  gpioInit();

  // Enable the LETIMER0 module and Si7021 temperature sensor over I2C
  LETIMER0_Enable(LOWEST_ENERGY_MODE);

#if BUILD_INCLUDES_BLE_SERVER == 1
  I2C_Init_Si7021();
#endif

  // Set the required parameters as per the desired energy mode
  switch(LOWEST_ENERGY_MODE){
    case EM0: break;
    case EM1: sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
            break;
    case EM2: sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM2);
            break;
    case EM3: break;
    default:break;
  }
} // app_init()

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
//  uint32_t event;
//
//  // Get the event to handle from the scheduler and call the state machine
//  event = getNextEvent();
//
//  temperature_state_machine(event);

} // app_process_action()

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *
 * The code here will process events from the Bluetooth stack. This is the only
 * opportunity we will get to act on an event.
 *
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{

   handle_ble_event(evt); // put this code in ble.c/.h

#if BUILD_INCLUDES_BLE_SERVER == 1
  // sequence through states driven by events
   temperature_state_machine_bt(evt);    // put this code in scheduler.c/.h
#endif

#if BUILD_INCLUDES_BLE_CLIENT == 1

   Discovery_State_Machine(evt);
#endif

} // sl_bt_on_event()

