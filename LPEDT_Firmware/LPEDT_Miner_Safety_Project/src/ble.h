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
 * @file    ble.h
 * @brief   Contains header information for ble.c
 *
 * @author  Vishnu Kumar Thoodur Venkatachalapathy
 * @date    Feb 18, 2024
 */

#ifndef SRC_BLE_H_
#define SRC_BLE_H_

#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "sl_bt_api.h"

// These are included here so that data types used below, and therefore are
// needed by the caller, are defined and available.
#include <stdint.h>    // for uint8_t etc.
#include <stdbool.h>   // for bool

// Helper Macros
#define UINT8_TO_BITSTREAM(p, n)  { *(p)++ = (uint8_t)(n); } // use this for the flags byte, which you set = 0
#define UINT32_TO_BITSTREAM(p, n) { *(p)++ = (uint8_t)(n); *(p)++ = (uint8_t)((n) >> 8); \
                                    *(p)++ = (uint8_t)((n) >> 16); *(p)++ = (uint8_t)((n) >> 24); }
#define INT32_TO_FLOAT(m, e)      ( (int32_t) (((uint32_t) m) & 0x00FFFFFFU) | (((uint32_t) e) << 24) )

#define SOFT_TIMER_0 0
#define SOFT_TIMER_TICK_VALUE_1SEC 32768 // Value given in soft_timer API documentation

#define SOFT_TIMER_1 1
#define SOFT_TIMER_TICK_VALUE_200ms 6554 // Value derived from soft_timer API documentation

// BLE Data Structure
typedef struct {
  // values that are common to servers and clients
  bd_addr myAddress;
  uint8_t myAddressType;

  uint8_t connectionHandle;
  uint8_t softTimerHandle;
  bool bondingStatus;

  // values unique for server
  uint8_t advertisingSetHandle;

  bool connection_open; // true when in an open connection
  bool ok_to_send_htm_indications; // true when client enabled indications
  bool ok_to_send_button_indications; // true when client enabled indications for button characteristics
  bool indication_in_flight; // true when an indication is in-flight

  // values unique for client
  uint32_t serviceHandle;    /**< service handle */
  uint16_t characteristicHandle; /**< GATT characteristic handle */

  uint32_t serviceHandleHTM;    /**< service handle */
  uint32_t serviceHandleButton;    /**< service handle */

  uint16_t characteristicHandleHTM;
  uint16_t characteristicHandleButton;

  uint16_t resultGATTProcedue;

  bool isIndicationOnButton;
  bool ok_to_send_button_read;

} ble_data_struct_t;

/**
 * @brief   Function to access the ble data structure
 * @return  a pointer to the ble data structure
 */
ble_data_struct_t* get_ble_data_ptr();

/**
 * @brief   Bluetooth event responder
 * @param   evt An bluetooth event to handle with data type 'sl_bt_msg_t'
 * @return  none
 */
void handle_ble_event(sl_bt_msg_t *evt);


// This is the number of entries in the queue. Please leave
// this value set to 16.
#define QUEUE_DEPTH      (16)

// Student edit:
//   define this to 1 if your design uses all array entries
//   define this to 0 if your design leaves 1 array entry empty
#define USE_ALL_ENTRIES  (1)

#define MAX_BUFFER_LENGTH  (5)
#define MIN_BUFFER_LENGTH  (1)

typedef struct {

  uint16_t       charHandle;                 // GATT DB handle from gatt_db.h
  uint32_t       bufLength;                  // Number of bytes written to field buffer[5]
  uint8_t        buffer[MAX_BUFFER_LENGTH];  // The actual data buffer for the indication,
                                             //   need 5-bytes for HTM and 1-byte for button_state.
                                             //   For testing, test lengths 1 through 5,
                                             //   a length of 0 shall be considered an
                                             //   error, as well as lengths > 5

} queue_struct_t;

// Function prototypes. The autograder (i.e. the testbench) only uses these
// functions to test your design. Please do not change these definitions or
// the autograder will fail.
void     reset_queue      (void);
bool     write_queue      (uint16_t  charHandle, uint32_t  bufLength, uint8_t *buffer);
bool     read_queue       (uint16_t *charHandle, uint32_t *bufLength, uint8_t *buffer);
void     get_queue_status (uint32_t *wptr, uint32_t *rptr, bool *full, bool *empty);
uint32_t get_queue_depth  (void);

#endif /* SRC_BLE_H_ */
