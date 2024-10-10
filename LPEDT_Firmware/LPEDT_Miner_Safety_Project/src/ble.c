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
 * @file    ble.c
 * @brief   Contains Bluetooth drivers
 *
 * @author  Vishnu Kumar Thoodur Venkatachalapathy
 * @date    Feb 18, 2024
 */
#include "ble.h"
#include "ble_device_type.h"
#include "lcd.h"
#include "scheduler.h"
#include "gpio.h"
#include <math.h>
#include <string.h> // for memcpy()


// Include logging for this file
#define INCLUDE_LOG_DEBUG 1
#include "log.h"

// following macros values are assigned by referring sl_bt_api.h
#define PUBLIC_ADDRESS 0
#define SCAN_PASSIVE 0

#define BUTTON_ON 0x01
#define BUTTON_OFF 0x00
/*
 * bit 0-2 : Connectable scannable, undirected advertising-> 000
 * bit 3-4 : Future Use -> 00
 * bit 5-6 : Data Complete -> 00
 * bit 7   : Legacy advertising PDUs used ->0
 */
#define PACKET_TYPE  0

#if BUILD_INCLUDES_BLE_CLIENT == 1
bd_addr serverAddress = SERVER_BT_ADDRESS;
bd_addr scannerAddress;
#endif

// BLE private data
ble_data_struct_t ble_data;

/**
 * @brief   Function to access the ble data structure
 * @return  a pointer to the ble data structure
 */
ble_data_struct_t* get_ble_data_ptr(){
  return (&ble_data);
}

#if BUILD_INCLUDES_BLE_CLIENT == 1

// state machine state used to toggle the indications for custom button
// characteristic
typedef enum uint16_t{
  Button_State_1,
  Button_State_2,
  Button_State_3,
  Button_State_4
} Button_States_t;

#endif


#if BUILD_INCLUDES_BLE_SERVER == 1

// Declare memory for the queue/buffer, and our write and read pointers.
queue_struct_t   my_queue[QUEUE_DEPTH]; // the queue
uint32_t         wptr = 0;              // write pointer
uint32_t         rptr = 0;              // read pointer

// Student edit:
// Decide how will you handle the full condition.

bool         queue_full = false;  // flag to denote the queue is full
bool         queue_empty = true;  // flag to denote the queue is empty



// ---------------------------------------------------------------------
// Private function used only by this .c file.
// Compute the next ptr value. Given a valid ptr value, compute the next valid
// value of the ptr and return it.
// Isolation of functionality: This defines "how" a pointer advances.
// ---------------------------------------------------------------------
static uint32_t nextPtr(uint32_t ptr) {

  // Student edit:
  // Create this function
  if(ptr+1 == QUEUE_DEPTH)
    return 0;
  else
    return ptr+1;

} // nextPtr()

// ---------------------------------------------------------------------
// Public function.
// This function resets the queue.
// ---------------------------------------------------------------------
void reset_queue (void) {

  // Student edit:
  // Create this function

  // Creating an array with value 0 which can be used to overwrite any existing
  // data in the queue
  uint8_t reset_buffer[MAX_BUFFER_LENGTH];
  for(uint32_t i=0; i<MAX_BUFFER_LENGTH; i++){
    reset_buffer[i] = 0;
  }

  // Iterate through the queue and reset all variables
  for(uint32_t i=0; i<QUEUE_DEPTH; i++){
    my_queue[i].charHandle = 0;
    my_queue[i].bufLength = 0;
    memcpy(&my_queue[i].buffer, &reset_buffer, MAX_BUFFER_LENGTH);
  }

  // reset the read/write pointers and queue flags
  wptr = 0;
  rptr = 0;
  queue_full = false;
  queue_empty = true;

} // reset_queue()

// ---------------------------------------------------------------------
// Public function.
// This function writes an entry to the queue if the the queue is not full.
// Input parameter "charHandle" should be written to queue_struct_t element "charHandle".
// Input parameter "bufLength" should be written to queue_struct_t element "bufLength"
// The bytes pointed at by input parameter "buffer" should be written to queue_struct_t element "buffer"
// Returns bool false if successful or true if writing to a full fifo.
// i.e. false means no error, true means an error occurred.
// ---------------------------------------------------------------------
bool write_queue (uint16_t charHandle, uint32_t bufLength, uint8_t *buffer) {

  // Student edit:
  // Create this function
  // Decide how you want to handle the "full" condition.

  // Don't forget to range check bufLength.
  // Isolation of functionality:
  //     Create the logic for "when" a pointer advances.

  // Check if the queue is full and return error if true
  if(queue_full)
    return true;

  // Check if the given buffer length is within the defined limits
  if((bufLength > MAX_BUFFER_LENGTH) || (bufLength < MIN_BUFFER_LENGTH))
    return true;

  // Set the queue full flag if the queue would be full after the current data
  // insertion
  if(nextPtr(wptr) == rptr){
    queue_full = true;
  }

  // Insert the data
  my_queue[wptr].charHandle = charHandle;
  my_queue[wptr].bufLength = bufLength;

  for(uint32_t i=0; i<bufLength; i++){
    my_queue[wptr].buffer[i] = buffer[i];
  }

  if(queue_empty)
    queue_empty = false;

  // Increment the write pointer if the queue is not full
  if(!queue_full)
    wptr = nextPtr(wptr);

  return false;
} // write_queue()

// ---------------------------------------------------------------------
// Public function.
// This function reads an entry from the queue, and returns values to the
// caller. The values from the queue entry are returned by writing
// the values to variables declared by the caller, where the caller is passing
// in pointers to charHandle, bufLength and buffer. The caller's code will look like this:
//
//   uint16_t     charHandle;
//   uint32_t     bufLength;
//   uint8_t      buffer[5];
//
//   status = read_queue (&charHandle, &bufLength, &buffer[0]);
//
// *** If the code above doesn't make sense to you, you probably lack the
// necessary prerequisite knowledge to be successful in this course.
//
// Write the values of charHandle, bufLength, and buffer from my_queue[rptr] to
// the memory addresses pointed at by charHandle, bufLength and buffer, like this :
//      *charHandle = <something>;
//      *bufLength  = <something_else>;
//      *buffer     = <something_else_again>; // perhaps memcpy() would be useful?
//
// In this implementation, we do it this way because
// standard C does not provide a mechanism for a C function to return multiple
// values, as is common in perl or python.
// Returns bool false if successful or true if reading from an empty fifo.
// i.e. false means no error, true means an error occurred.
// ---------------------------------------------------------------------
bool read_queue (uint16_t *charHandle, uint32_t *bufLength, uint8_t *buffer) {

  // Student edit:
  // Create this function

  // Isolation of functionality:
  //     Create the logic for "when" a pointer advances

  // Check if the queue is empty and return error if true
  if(queue_empty)
    return true;

  // Set the queue empty flag if the queue shall be empty after the current read
  // operation
  if(nextPtr(rptr) == wptr)
    queue_empty = true;

  // Copy the data from queue to the user arguments
  *charHandle = my_queue[rptr].charHandle;
  *bufLength = my_queue[rptr].bufLength;
  memcpy(buffer, &my_queue[rptr].buffer, my_queue[rptr].bufLength);

  // Increment the read pointer and if the queue is full, clear that flag and
  // increment the write pointer
  rptr = nextPtr(rptr);

  if(queue_full){
    queue_full = false;
    wptr = nextPtr(wptr);
  }

  return false;
} // read_queue()

// ---------------------------------------------------------------------
// Public function.
// This function returns the wptr, rptr, full and empty values, writing
// to memory using the pointer values passed in, same rationale as read_queue()
// The "_" characters are used to disambiguate the global variable names from
// the input parameter names, such that there is no room for the compiler to make a
// mistake in interpreting your intentions.
// ---------------------------------------------------------------------
void get_queue_status (uint32_t *_wptr, uint32_t *_rptr, bool *_full, bool *_empty) {

  // Student edit:
  // Create this function

  *_wptr = wptr;
  *_rptr = rptr;
  *_full = queue_full;
  *_empty = queue_empty;

} // get_queue_status()

// ---------------------------------------------------------------------
// Public function.
// Function that computes the number of written entries currently in the queue. If there
// are 3 entries in the queue, it should return 3. If the queue is empty it should
// return 0. If the queue is full it should return either QUEUE_DEPTH if
// USE_ALL_ENTRIES==1 otherwise returns QUEUE_DEPTH-1.
// ---------------------------------------------------------------------
uint32_t get_queue_depth() {

  // Student edit:
  // Create this function
  if(queue_empty)
    return 0;
  else if(queue_full)
    return QUEUE_DEPTH;
  else{
    uint32_t queue_depth_ctr = 0, i=rptr;
    while(i != wptr){
      queue_depth_ctr++;
      i = nextPtr(i);
    }
    return queue_depth_ctr;
  }

} // get_queue_depth()

#endif

#if BUILD_INCLUDES_BLE_CLIENT == 1
// -----------------------------------------------
// Private function, original from Dan Walkes. I fixed a sign extension bug.
// We'll need this for Client A7 assignment to convert health thermometer
// indications back to an integer. Convert IEEE-11073 32-bit float to signed integer.
// -----------------------------------------------
static int32_t FLOAT_TO_INT32(const uint8_t *buffer_ptr)
{
  uint8_t signByte = 0;
  int32_t mantissa;
  // input data format is:
  // [0] = flags byte, bit[0] = 0 -> Celsius; =1 -> Fahrenheit
  // [3][2][1] = mantissa (2's complement)
  // [4] = exponent (2's complement)
  // BT buffer_ptr[0] has the flags byte
  int8_t exponent = (int8_t)buffer_ptr[4];
  // sign extend the mantissa value if the mantissa is negative
  if (buffer_ptr[3] & 0x80) { // msb of [3] is the sign of the mantissa
      signByte = 0xFF;
  }
  mantissa = (int32_t) (buffer_ptr[1] << 0) |
                       (buffer_ptr[2] << 8) |
                       (buffer_ptr[3] << 16) |
                       (signByte << 24) ;

  // value = 10^exponent * mantissa, pow() returns a double type
  return (int32_t) (pow(10, exponent) * mantissa);

} // FLOAT_TO_INT32
#endif

/**
 * @brief   Bluetooth event responder
 * @param   evt An bluetooth event to handle with data type 'sl_bt_msg_t'
 * @return  none
 */
void handle_ble_event(sl_bt_msg_t *evt){
  sl_status_t sc; // status code
  ble_data_struct_t *ble_data = get_ble_data_ptr();
  static bool confirm_flag = false;

#if BUILD_INCLUDES_BLE_CLIENT == 1
  int32_t temp_data = 0;
#endif

  switch (SL_BT_MSG_ID(evt->header)) {

  /*  ------------------------------------------------------------------------
   *  Events common to both server and client:
   *    sl_bt_evt_system_boot_id
   *    sl_bt_evt_connection_opened_id
   *    sl_bt_evt_connection_closed_id
   *    sl_bt_evt_system_soft_timer_id
   *    sl_bt_evt_sm_confirm_bonding_id
   *    sl_bt_evt_sm_confirm_passkey_id
   *    sl_bt_evt_sm_bonded_id
   *    sl_bt_evt_sm_bonding_failed_id
   *  ------------------------------------------------------------------------
   */

    // This event indicates the device has started and the radio is ready.
    case sl_bt_evt_system_boot_id:

      // Get system address
      sc = sl_bt_system_get_identity_address(&ble_data->myAddress, &ble_data->myAddressType);
      if (sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_system_get_identity_address() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }

#if BUILD_INCLUDES_BLE_SERVER == 1
      // Get ble_data.advertisingSetHandle
      sc = sl_bt_advertiser_create_set(&ble_data->advertisingSetHandle);
      if (sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_advertiser_create_set() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }

      // Setting the Advertising minimum and maximum to 250mS
      sc = sl_bt_advertiser_set_timing(
              ble_data->advertisingSetHandle, // advertising set handle
              400, // min. adv. interval (milliseconds * 1.6)
              400, // max. adv. interval (milliseconds * 1.6)
              0,   // adv. duration
              0);  // max. num. adv. events
      if (sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_advertiser_set_timing() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }

      // Starting advertiser
      sc = sl_bt_advertiser_start(
              ble_data->advertisingSetHandle,
              sl_bt_advertiser_general_discoverable,
              sl_bt_advertiser_connectable_scannable);
      if (sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_advertiser_start() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }

      sc = sl_bt_system_set_soft_timer(SOFT_TIMER_TICK_VALUE_200ms, SOFT_TIMER_1, false);

      if(sc != SL_STATUS_OK){
          LOG_ERROR("sl_bt_system_set_soft_timer() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }
#endif
#if BUILD_INCLUDES_BLE_CLIENT == 1

      // Set phy to 1M and mode to passive scanning
      sc = sl_bt_scanner_set_mode(sl_bt_gap_1m_phy, SCAN_PASSIVE);

      if(sc != SL_STATUS_OK){
          LOG_ERROR("sl_bt_scanner_set_mode() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }

      // Set phy to 1M, scan interval to 50ms and scan window to 25ms
      sc = sl_bt_scanner_set_timing(
            sl_bt_gap_1m_phy, // PHYs for which the parameters are set
            80,  // Scan Interval. (milliseconds/0.625)
            40// Scan Window (milliseconds/0.625)
            );

      if(sc != SL_STATUS_OK){
          LOG_ERROR("sl_bt_scanner_set_timing() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }

      /*
       * connection parameters:
       *  a. min_interval = 75 ms
       *  b. max_interval = 75 ms
       *  c. slave_latency = 4
       *  d. Supervision timeout = (1 + slave_latency) * (max_interval * 2) + max_interval
       *                         = (1 + 4) * (75*2) + 75 = 825ms
       *  e. min_ce_length = 0 (recommended for current Gecko 3.x SDK)
       *  f. max_ce_length = 4
       */
      sc = sl_bt_connection_set_default_parameters(
          60,                           // min connection interval. (time in milliseconds / 1.25)
          60,                           // max connection interval. (time in milliseconds / 1.25)
          4,                            // num of connection intervals to skip
          84,                           // Supervision time out. (time in milliseconds / 10)
          0,                            // min_ce length
          4                             // max_ce length
          );

      if(sc != SL_STATUS_OK){
          LOG_ERROR("sl_bt_connection_set_default_parameters() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }
      // Start scanning
      sc = sl_bt_scanner_start(sl_bt_gap_1m_phy, sl_bt_scanner_discover_generic);

      if(sc != SL_STATUS_OK){
          LOG_ERROR("sl_bt_scanner_start() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }
#endif
      // Initialization of connection parameters
      ble_data->connection_open = false;
      ble_data->indication_in_flight = false;
      ble_data->ok_to_send_htm_indications = false;
      ble_data->ok_to_send_button_indications = false;
      ble_data->bondingStatus = false;
      ble_data->ok_to_send_button_read = true;

      // delete all previous bonding
      sc = sl_bt_sm_delete_bondings();

      if(sc != SL_STATUS_OK){
          LOG_ERROR("sl_bt_sm_delete_bondings() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }

      // Init the bonding parameters
      /** flags configuration:
       *   Bit 0: 1:</b> Bonding requires authentication (Man-in-the-Middle
       *       protection)
       *
       *   Bit 1: 1:</b> Encryption requires bonding. Note that this setting will also
       *       enable bonding.
       *
       *   Bit 2: 0:</b> Allow bonding with legacy pairing
       *
       *   Bit 3: 1:</b> Bonding requests need to be confirmed. Received bonding
       *       requests are notified by @ref sl_bt_evt_sm_confirm_bonding
       *
       *   Bit 4: 0:</b> Allow all connections
       *
       *   Bit 5: 1:</b> Prefer authenticated pairing when both options are possible
       *       based on the settings.
       *
       *   Bit 6 to 7: Reserved
       */
      uint8_t flags = 0b00101011;
      sc = sl_bt_sm_configure(flags, sl_bt_sm_io_capability_displayyesno);

      if(sc != SL_STATUS_OK){
          LOG_ERROR("sl_bt_sm_configure() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }

      // Initialize the LCD display and display all the informations as required.
      displayInit();
      displayPrintf(DISPLAY_ROW_NAME, BLE_DEVICE_TYPE_STRING); // Display Server/Client
      displayPrintf(DISPLAY_ROW_BTADDR, "%02X:%02X:%02X:%02X:%02X:%02X",
                    ble_data->myAddress.addr[0],
                    ble_data->myAddress.addr[1],
                    ble_data->myAddress.addr[2],
                    ble_data->myAddress.addr[3],
                    ble_data->myAddress.addr[4],
                    ble_data->myAddress.addr[5]);

      displayPrintf(DISPLAY_ROW_ASSIGNMENT, "A9");

#if BUILD_INCLUDES_BLE_SERVER == 1
      displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
      displayPrintf(DISPLAY_ROW_9, "Button Released");
#endif
#if BUILD_INCLUDES_BLE_CLIENT == 1
      displayPrintf(DISPLAY_ROW_CONNECTION, "Discovering");
#endif

      break;

    // This event indicates that the server is connected to a client
    case sl_bt_evt_connection_opened_id:

      //-----------------------------------------------------------------------
      // Values to save:
      //  Connection handle.
      //  Maintain a flag to tracks the connection.
      //-----------------------------------------------------------------------

      ble_data->connectionHandle = evt->data.evt_connection_opened.connection;
      ble_data->connection_open = true;
      ble_data->ok_to_send_button_read = true;

#if BUILD_INCLUDES_BLE_SERVER == 1
      // Stopping advertisement
      sc = sl_bt_advertiser_stop(ble_data->advertisingSetHandle);
      if (sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_advertiser_stop() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }

      // Setting Connection Interval minimum and maximum to 75mS
      // Setting the Slave latency to enable it to be “off the air” for up to 300mS
      // Setting Set the Supervision timeout to a value greater than (1 + slave latency) *
      // (connection_interval * 2). See API documentation.

      // Supervision timeout = (1 + 4) * (75 * 2) = 5 * 150 = 750ms.
      // Hence, setting value of 760ms for supervision timeout
      sc = sl_bt_connection_set_parameters(
            ble_data->connectionHandle,   // Connection Handle
            60,                           // min connection interval. (time in milliseconds / 1.25)
            60,                           // max connection interval. (time in milliseconds / 1.25)
            4,                            // num of connection intervals to skip
            84,                           // Supervision time out. (time in milliseconds / 10)
            0,                            // min_ce length
            0                             // max_ce length
            );

      if(sc != SL_STATUS_OK){
          LOG_ERROR("parm not set. sl_bt_connection_set_parameters() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }
#endif

#if BUILD_INCLUDES_BLE_CLIENT == 1
      sc = sl_bt_scanner_stop();

      if(sc != SL_STATUS_OK){
          LOG_ERROR("sl_bt_scanner_stop() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }
      displayPrintf(DISPLAY_ROW_BTADDR2, "%02X:%02X:%02X:%02X:%02X:%02X",
                      serverAddress.addr[0],
                      serverAddress.addr[1],
                      serverAddress.addr[2],
                      serverAddress.addr[3],
                      serverAddress.addr[4],
                      serverAddress.addr[5]);
#endif
      // update the connection status
      displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");
      break;

    // This event indicates that the connection between server and client is closed
    case sl_bt_evt_connection_closed_id:

      //-----------------------------------------------------------------------
      // Values to save:
      //  Maintain a flag to tracks the connection.
      //-----------------------------------------------------------------------

#if BUILD_INCLUDES_BLE_SERVER == 1
      // Restarting advertisement
      sc = sl_bt_advertiser_start(
              ble_data->advertisingSetHandle,
              sl_bt_advertiser_general_discoverable,
              sl_bt_advertiser_connectable_scannable);

      if(sc != SL_STATUS_OK){
          LOG_ERROR("sl_bt_advertiser_start() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }

      // update the connection status
      displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");

      // clear LED status
      gpioLed0SetOff();
      gpioLed1SetOff();
#endif

#if BUILD_INCLUDES_BLE_CLIENT == 1
      // Start scanning
      sc = sl_bt_scanner_start(sl_bt_gap_1m_phy, sl_bt_scanner_discover_generic);

      if(sc != SL_STATUS_OK){
          LOG_ERROR("sl_bt_scanner_start() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }
      displayPrintf(DISPLAY_ROW_BTADDR2, "");
      displayPrintf(DISPLAY_ROW_TEMPVALUE, "");
      displayPrintf(DISPLAY_ROW_9, "");
      displayPrintf(DISPLAY_ROW_CONNECTION, "Discovering");
#endif
      // Resetting connection parameters
      ble_data->connection_open = false;
      ble_data->indication_in_flight = false;
      ble_data->ok_to_send_htm_indications = false;
      ble_data->ok_to_send_button_indications = false;
      ble_data->bondingStatus = false;
      ble_data->isIndicationOnButton = false;
      ble_data->ok_to_send_button_read = true;

      // delete all previous bonding
      sc = sl_bt_sm_delete_bondings();

      if(sc != SL_STATUS_OK){
          LOG_ERROR("sl_bt_sm_delete_bondings() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }
      break;

    // This event indicates that some connection parameter has changed.
    // (we shall just use this event to verify the connection parameters once
    // by printing them out and comment it out for the final submission code
    case sl_bt_evt_connection_parameters_id:

      // print all connection parameters.
//      LOG_INFO("event: sl_bt_evt_connection_parameters_id\r\n");
//      LOG_INFO(" Connection Parameters:\r\n Connection: %d\r\n interval: %d\r\n latency: %d\r\n security_mode: %d\r\n timeout: %d\r\n txsize: %d\r\n",
//               (int) (evt->data.evt_connection_parameters.connection),
//               (int) (evt->data.evt_connection_parameters.interval),
//               (int) (evt->data.evt_connection_parameters.latency),
//               (int) (evt->data.evt_connection_parameters.security_mode),
//               (int) (evt->data.evt_connection_parameters.timeout),
//               (int) (evt->data.evt_connection_parameters.txsize)
//              );
      break;

      // This event indicates that an soft timer event has occurred
      case sl_bt_evt_system_soft_timer_id:

        // Since the soft timer supports multiple concurrent timers, we need to
        // check which timer has been triggered by examining the handle returned.
        switch(evt->data.evt_system_soft_timer.handle){
          case SOFT_TIMER_0:
              displayUpdate();
              break;

#if BUILD_INCLUDES_BLE_SERVER == 1
          case SOFT_TIMER_1:
            if(get_queue_depth()>0){
                queue_struct_t data;
                read_queue(&data.charHandle, &data.bufLength, &data.buffer[0]);
                // Server Sending the Indication.

                if(((data.charHandle == gattdb_button_state) && (ble_data->ok_to_send_button_indications == true))||
                   ((data.charHandle == gattdb_temperature_measurement) && (ble_data->ok_to_send_htm_indications == true))){
                  sc = sl_bt_gatt_server_send_indication(
                        ble_data->connectionHandle,
                        data.charHandle,
                        data.bufLength,
                        &data.buffer[0]
                       );
                  if (sc != SL_STATUS_OK) {
                      LOG_ERROR("sl_bt_gatt_server_send_indication() for button state returned != 0 status=0x%04x\r\n", (unsigned int) sc);
                  }
                }
            }
            break;
#endif
        }

        break;


    // This event indicates that sl_bt_external_signal(myEvent) was called.
    case sl_bt_evt_system_external_signal_id:


#if BUILD_INCLUDES_BLE_SERVER == 1
      if ((evt->data.evt_system_external_signal.extsignals & (1<<PB0_BIT_POS))){
          uint8_t button_state_buffer[1];
          if(Get_PB0_State()){
              if(confirm_flag){
                sc = sl_bt_sm_passkey_confirm(ble_data->connectionHandle, 1);
                confirm_flag = false;
                if(sc != SL_STATUS_OK){
                    LOG_ERROR("sl_bt_scanner_start() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
                }
              }

              displayPrintf(DISPLAY_ROW_9, "Button Pressed");
              button_state_buffer[0] = BUTTON_ON;
          }
          else{
              button_state_buffer[0] = BUTTON_OFF;
              displayPrintf(DISPLAY_ROW_9, "Button Released");
          }

          sc = sl_bt_gatt_server_write_attribute_value(
                            gattdb_button_state,
                            0,
                            1,
                            &button_state_buffer[0]);
          if(sc != SL_STATUS_OK){
              LOG_ERROR("sl_bt_gatt_server_write_attribute_value() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
          }

          if  ((ble_data->connection_open == true) &&
               (ble_data->ok_to_send_button_indications == true) &&
               (ble_data->bondingStatus == true)){

              if(!((ble_data->indication_in_flight == false)||
                  (get_queue_depth() > 0))){
                  // Server Sending the Indication.
                  sc = sl_bt_gatt_server_send_indication(
                        ble_data->connectionHandle,
                        gattdb_button_state, // handle from gatt_db.h
                        1,
                        &button_state_buffer[0]
                       );
                  if (sc != SL_STATUS_OK) {
                      LOG_ERROR("sl_bt_gatt_server_send_indication() for button state returned != 0 status=0x%04x\r\n", (unsigned int) sc);
                  }
                  ble_data->indication_in_flight = true;
              } // if
              else{
                  write_queue(gattdb_button_state, 1, &button_state_buffer[0]);
              }
          }// if
      }
#endif

#if BUILD_INCLUDES_BLE_CLIENT == 1
      if ((evt->data.evt_system_external_signal.extsignals & (1<<PB0_BIT_POS)) ||
          (evt->data.evt_system_external_signal.extsignals & (1<<PB1_BIT_POS))){
          Button_States_t currentState;
          static Button_States_t nextState = Button_State_1;

          currentState = nextState;
          if((confirm_flag == true) && (Get_PB0_State() == true)){
            sc = sl_bt_sm_passkey_confirm(ble_data->connectionHandle, 1);
            confirm_flag = false;
            if(sc != SL_STATUS_OK){
                LOG_ERROR("sl_bt_scanner_start() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
            }
          }

          switch(currentState){
            case Button_State_1:
                nextState = Button_State_1;
                if((Get_PB0_State() == true)&&(Get_PB1_State() == false)){
                  nextState = Button_State_2;
                }

                if(((Get_PB0_State() == false)&&(Get_PB1_State() == true)&&(ble_data->ok_to_send_button_read == true))){
                  sc = sl_bt_gatt_read_characteristic_value(ble_data->connectionHandle,
                                                            ble_data->characteristicHandleButton);
                  ble_data->ok_to_send_button_read = false;
                  if(sc != SL_STATUS_OK){
                      LOG_ERROR("sl_bt_gatt_read_characteristic_value() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
                  }
                }
                break;
            case Button_State_2:
                nextState = Button_State_2;
                if((Get_PB0_State() == true)&&(Get_PB1_State() == true)){
                  nextState = Button_State_3;
                }
                if(Get_PB0_State() == false){
                  nextState = Button_State_1;
                }
                break;
            case Button_State_3:
                nextState = Button_State_3;
                if((Get_PB0_State() == true)&&(Get_PB1_State() == false)){
                  nextState = Button_State_4;
                }
                if(Get_PB0_State() == false){
                  nextState = Button_State_1;
                }
                break;
            case Button_State_4:
                nextState = Button_State_4;
                if((Get_PB0_State() == false)&&(Get_PB1_State() == false)){
                  nextState = Button_State_1;
                  // toggle the indication cmd and send it
                  if(ble_data->isIndicationOnButton == true){
                      sc = sl_bt_gatt_set_characteristic_notification(ble_data->connectionHandle,
                                                                      ble_data->characteristicHandleButton,
                                                                      sl_bt_gatt_disable);
                      ble_data->isIndicationOnButton = false;
                  }
                  else{
                      sc = sl_bt_gatt_set_characteristic_notification(ble_data->connectionHandle,
                                                                      ble_data->characteristicHandleButton,
                                                                      sl_bt_gatt_indication);
                      ble_data->isIndicationOnButton = true;
                  }
                  if(sc != SL_STATUS_OK){
                      LOG_ERROR("sl_bt_gatt_read_characteristic_value() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
                  }
                }
                break;
          }
      }
#endif
      break;

    case sl_bt_evt_sm_confirm_bonding_id:
      sc = sl_bt_sm_bonding_confirm(ble_data->connectionHandle, 1);

      if(sc != SL_STATUS_OK){
          LOG_ERROR("sl_bt_sm_bonding_confirm() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
      }

      break;
    case sl_bt_evt_sm_confirm_passkey_id:

      displayPrintf(DISPLAY_ROW_PASSKEY, "Passkey %06d", evt->data.evt_sm_confirm_passkey.passkey);
      displayPrintf(DISPLAY_ROW_ACTION, "Confirm with PB0");
      confirm_flag = true;
      break;

    case sl_bt_evt_sm_bonded_id:
      displayPrintf(DISPLAY_ROW_PASSKEY, " ");
      displayPrintf(DISPLAY_ROW_ACTION, " ");
      displayPrintf(DISPLAY_ROW_CONNECTION, "Bonded");
      ble_data->bondingStatus = true;
      break;
    case sl_bt_evt_sm_bonding_failed_id:
      displayPrintf(DISPLAY_ROW_PASSKEY, " ");
      displayPrintf(DISPLAY_ROW_ACTION, " ");
      ble_data->bondingStatus = false;
      break;
    /*  ------------------------------------------------------------------------
    *  Server Events:
    *    sl_bt_evt_system_external_signal_id
    *    sl_bt_evt_gatt_server_characteristic_status_id
    *    sl_bt_evt_gatt_server_indication_timeout_id
    *
    *  ------------------------------------------------------------------------
    */
#if BUILD_INCLUDES_BLE_SERVER == 1

    // This event indicates that either a CCCD has been changed by the GATT
    // client or we have received a confirmation from the remote GATT Client
    // was received upon a successful reception of the indication
    case sl_bt_evt_gatt_server_characteristic_status_id:
      //-----------------------------------------------------------------------
      // Values to save:
      //  Track whether indications are enabled/disabled for each and every
      // characteristic. (Indications for each characteristic value is
      // independently controllable from the client.)
      //
      // Track whether an indication is in flight or not.
      //
      //-----------------------------------------------------------------------

      // Check if the event is related to the htm characteristic and if change
      // is done by the GATT client.
      if (evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_temperature_measurement
          && evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_client_config)
      {
          // Get the status of the GATT server characteristic config. flag and
          // modify the internal tracking flag appropriately
          if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_server_disable){
            ble_data->ok_to_send_htm_indications = false;
            gpioLed0SetOff();
          }

          if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_server_indication){
            ble_data->ok_to_send_htm_indications = true;
            gpioLed0SetOn();
          }
      }

      // Check if the event is related to the button characteristic and if change
      // is done by the GATT client.
      if (evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_button_state
          && evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_client_config)
      {
          // Get the status of the GATT server characteristic config. flag and
          // modify the internal tracking flag appropriately
          if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_server_disable){
            ble_data->ok_to_send_button_indications = false;
            gpioLed1SetOff();
          }

          if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_server_indication){
            ble_data->ok_to_send_button_indications = true;
            gpioLed1SetOn();
          }
      }

      // Check if the event is related to the htm or the custom button characteristic and if we
      // received confirmation of reception from GATT client for a previously
      // transmitted indication.
      if(((evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_temperature_measurement) ||
          (evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_button_state))&&
         (evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_confirmation)){
         ble_data->indication_in_flight = false; //indication reached
      }

      break;

    // This event indicates that we never received a confirmation for a
    // previously transmitted indication.
    case sl_bt_evt_gatt_server_indication_timeout_id:
      //-----------------------------------------------------------------------
      // Values to save:
      // Track whether an indication is in flight or not.
      //-----------------------------------------------------------------------
      ble_data->indication_in_flight = false;
      LOG_ERROR("event: sl_bt_evt_gatt_server_indication_timeout_id\r\n Parameters:\r\n Connection: %d\r\n",
                     (int) (evt->data.evt_gatt_server_indication_timeout.connection)
              );

      break;
#endif

#if BUILD_INCLUDES_BLE_CLIENT == 1
    /*  ------------------------------------------------------------------------
    *  Client Events:
    *    sl_bt_evt_scanner_scan_report_id
    *    sl_bt_evt_gatt_procedure_completed_id
    *    sl_bt_evt_gatt_service_id
    *    sl_bt_evt_gatt_characteristic_id
    *    sl_bt_evt_gatt_characteristic_value_id
    *  ------------------------------------------------------------------------
    */
    case sl_bt_evt_scanner_scan_report_id:
        // Is the bd_addr, packet_type and address_type what we expect for our Server? If yes, call sl_bt_connection_open()

        scannerAddress = evt->data.evt_scanner_scan_report.address;
        uint32_t addressMatchFlag = 1;
        for(int i=0; i<6; i++){
            if (scannerAddress.addr[i] != serverAddress.addr[i]){
                addressMatchFlag = 0;
                break;
            }
        }

        if ((addressMatchFlag == 1) &&
            (evt->data.evt_scanner_scan_report.address_type == PUBLIC_ADDRESS)&&
            (evt->data.evt_scanner_scan_report.packet_type == PACKET_TYPE)){
            sc = sl_bt_connection_open(evt->data.evt_scanner_scan_report.address,
                                       evt->data.evt_scanner_scan_report.address_type,
                                       sl_bt_gap_phy_1m,
                                       NULL
                                       );

            if(sc != SL_STATUS_OK){
                LOG_ERROR("sl_bt_connection_open() returned != 0 status=0x%04x\r\n", (unsigned int) sc);
            }
        }
          break;

    case sl_bt_evt_gatt_procedure_completed_id:
      // Save the result field in the ble private data structure which is returned from this event (might be useful later on)
      ble_data->resultGATTProcedue = evt->data.evt_gatt_procedure_completed.result;
      if(ble_data->resultGATTProcedue == SL_STATUS_BT_ATT_INSUFFICIENT_ENCRYPTION){
          sc = sl_bt_sm_increase_security(ble_data->connectionHandle);
      }
      break;

    case sl_bt_evt_gatt_service_id:
      //Save - Service Handle
      ble_data->serviceHandle = evt->data.evt_gatt_service.service;
      break;

    case sl_bt_evt_gatt_characteristic_id:
      // Save - Characteristic Handle
      ble_data->characteristicHandle = evt->data.evt_gatt_characteristic.characteristic;
      break;

    case sl_bt_evt_gatt_characteristic_value_id:
      /*
       * Is the characteristic handle and att_opcode we expect? If yes:
       *  sl_bt_gatt_send_characteristic_confirmation()
       *
       *  if the characteristic handle from the event is for the HTM characteristic,
       *    Show temperature on the LCD display as per the given scale on row
       *    'DISPLAY_ROW_TEMPVALUE' similar to that in server build
       *
       *  if the characteristic handle from the event is for the button characteristic,
       *    if the returned value is 0x00, then display text "Button Released"
       *    on row 'DISPLAY_ROW_9' in the LCD..
       *
       *    if the returned value is 0x01, then display text "Button Pressed"
       *    on row 'DISPLAY_ROW_9' in the LCD..
       */
      if((evt->data.evt_gatt_characteristic_value.characteristic == ble_data->characteristicHandleHTM)&&
          (evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_handle_value_indication)){
          sc = sl_bt_gatt_send_characteristic_confirmation(evt->data.evt_gatt_characteristic_value.connection);

          uint8_t *GATT_char_value = &(evt->data.evt_gatt_characteristic_value.value.data[0]);
          temp_data = FLOAT_TO_INT32(GATT_char_value);
          displayPrintf(DISPLAY_ROW_TEMPVALUE, "Temp=%d", temp_data);
      }

      if((evt->data.evt_gatt_characteristic_value.characteristic == ble_data->characteristicHandleButton)&&
          ((evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_handle_value_indication)||
           (evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_read_response))){
          sc = sl_bt_gatt_send_characteristic_confirmation(evt->data.evt_gatt_characteristic_value.connection);
          ble_data->ok_to_send_button_read = true;
          uint8_t *GATT_char_value = &(evt->data.evt_gatt_characteristic_value.value.data[0]);
          if(GATT_char_value[0] == BUTTON_OFF){
              displayPrintf(DISPLAY_ROW_9, "Button Released");
          }
          if(GATT_char_value[0] == BUTTON_ON){
              displayPrintf(DISPLAY_ROW_9, "Button Pressed");
          }
      }

      break;

#endif

  } // end - switch

} // handle_ble_event()
