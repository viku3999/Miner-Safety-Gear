/***************************************************************************//**
 * @brief SL_BT_API command declarations
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/


#ifndef SL_BT_API_H
#define SL_BT_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "sl_status.h"
#include "sl_bgapi.h"
#include "sl_bt_api_compatibility.h"


/* SL_BT_ synonyms for BGAPI header macros */
#define SL_BT_MSG_ID(HDR)        SL_BGAPI_MSG_ID(HDR)
#define SL_BT_MSG_HEADER_LEN     SL_BGAPI_MSG_HEADER_LEN
#define SL_BT_MSG_LEN(HDR)       SL_BGAPI_MSG_LEN(HDR)
#define SL_BT_BIT_ENCRYPTED      SL_BGAPI_BIT_ENCRYPTED
#define SL_BT_MSG_ENCRYPTED(HDR) SL_BGAPI_MSG_ENCRYPTED(HDR)

/**
 * @addtogroup sl_bt_common_types BT Common Types
 * @{
 *
 * @brief BT common types
 */

/**
 * @brief Value used to indicate an invalid bonding handle
 */
#define SL_BT_INVALID_BONDING_HANDLE ((uint8_t) 0xFF)

/**
 * @brief Value used to indicate an invalid advertising set handle
 */
#define SL_BT_INVALID_ADVERTISING_SET_HANDLE ((uint8_t) 0xFF)

/** @} */ // end addtogroup sl_bt_common_types


/**
 * @addtogroup sl_bt_dfu Device Firmware Update
 * @{
 *
 * @brief Device Firmware Update
 *
 * These commands and events are related to controlling firmware updates over
 * the configured host interface and are available only when the device is
 * booted in DFU mode. <b>DFU process:</b>
 *   1. Boot device to DFU mode with @ref sl_bt_dfu_reset command
 *   2. Wait for @ref sl_bt_evt_dfu_boot event
 *   3. Send command @ref sl_bt_dfu_flash_set_address command to start the
 *      firmware update
 *   4. Upload the firmware with @ref sl_bt_dfu_flash_upload commands until all
 *      data is uploaded
 *   5. Send @ref sl_bt_dfu_flash_upload_finish command when all data is
 *      uploaded
 *   6. Finalize DFU firmware update with @ref sl_bt_dfu_reset command
 *
 * DFU mode is using the UART baudrate set in bootloader.
 */

/* Command and Response IDs */
#define sl_bt_cmd_dfu_reset_id                                       0x00000020
#define sl_bt_cmd_dfu_flash_set_address_id                           0x01000020
#define sl_bt_cmd_dfu_flash_upload_id                                0x02000020
#define sl_bt_cmd_dfu_flash_upload_finish_id                         0x03000020
#define sl_bt_rsp_dfu_reset_id                                       0x00000020
#define sl_bt_rsp_dfu_flash_set_address_id                           0x01000020
#define sl_bt_rsp_dfu_flash_upload_id                                0x02000020
#define sl_bt_rsp_dfu_flash_upload_finish_id                         0x03000020

/**
 * @addtogroup sl_bt_evt_dfu_boot sl_bt_evt_dfu_boot
 * @{
 * @brief This event indicates that the device booted in DFU mode and is now
 * ready to receive commands related to device firmware upgrade (DFU).
 */

/** @brief Identifier of the boot event */
#define sl_bt_evt_dfu_boot_id                                        0x000000a0

/***************************************************************************//**
 * @brief Data structure of the boot event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_dfu_boot_s
{
  uint32_t version; /**< The version of the bootloader */
});

typedef struct sl_bt_evt_dfu_boot_s sl_bt_evt_dfu_boot_t;

/** @} */ // end addtogroup sl_bt_evt_dfu_boot

/**
 * @addtogroup sl_bt_evt_dfu_boot_failure sl_bt_evt_dfu_boot_failure
 * @{
 * @brief This event indicates that an error, which prevents the device from
 * booting, has occurred in bootloader.
 */

/** @brief Identifier of the boot_failure event */
#define sl_bt_evt_dfu_boot_failure_id                                0x010000a0

/***************************************************************************//**
 * @brief Data structure of the boot_failure event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_dfu_boot_failure_s
{
  uint16_t reason; /**< The reason for boot failure. */
});

typedef struct sl_bt_evt_dfu_boot_failure_s sl_bt_evt_dfu_boot_failure_t;

/** @} */ // end addtogroup sl_bt_evt_dfu_boot_failure

/***************************************************************************//**
 *
 * Reset the system. The command does not have a response but it triggers one of
 * the boot events (normal reset or boot to DFU mode) after re-boot.
 *
 * @param[in] dfu Enum @ref sl_bt_system_boot_mode_t. Boot mode. Values:
 *     - <b>sl_bt_system_boot_mode_normal (0x0):</b> Boot to normal mode
 *     - <b>sl_bt_system_boot_mode_uart_dfu (0x1):</b> Boot to UART DFU mode
 *     - <b>sl_bt_system_boot_mode_ota_dfu (0x2):</b> Boot to OTA DFU mode
 *
 * @b Events
 *   - @ref sl_bt_evt_system_boot - Sent after the device has booted in normal
 *     mode
 *   - @ref sl_bt_evt_dfu_boot - Sent after the device has booted in UART DFU
 *     mode
 *
 ******************************************************************************/
void sl_bt_dfu_reset(uint8_t dfu);

/***************************************************************************//**
 *
 * After re-booting the local device in DFU mode, this command defines the
 * starting address on the flash where the new firmware will be written.
 *
 * @param[in] address The offset in the flash where the new firmware is uploaded
 *   to. Always use the value 0x00000000.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_dfu_flash_set_address(uint32_t address);

/***************************************************************************//**
 *
 * Upload the whole firmware image file into the Bluetooth device. The passed
 * data length must be a multiple of 4 bytes. Because the BGAPI command payload
 * size is limited, multiple commands need to be issued one after the other
 * until the whole firmware image file is uploaded to the device. After each
 * command, the next address of the flash sector in memory to write to is
 * automatically updated by the bootloader.
 *
 * @param[in] data_len Length of data in @p data
 * @param[in] data An array of data which will be written onto the flash.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_dfu_flash_upload(size_t data_len, const uint8_t* data);

/***************************************************************************//**
 *
 * Inform the device that the DFU file is fully uploaded. To return the device
 * back to normal mode, issue the command @ref sl_bt_dfu_reset.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_dfu_flash_upload_finish();

/** @} */ // end addtogroup sl_bt_dfu

/**
 * @addtogroup sl_bt_system System
 * @{
 *
 * @brief System
 *
 * Commands and events in this class can be used to access and query the local
 * device.
 */

/* Command and Response IDs */
#define sl_bt_cmd_system_hello_id                                    0x00010020
#define sl_bt_cmd_system_start_bluetooth_id                          0x1c010020
#define sl_bt_cmd_system_stop_bluetooth_id                           0x1d010020
#define sl_bt_cmd_system_get_version_id                              0x1b010020
#define sl_bt_cmd_system_reset_id                                    0x01010020
#define sl_bt_cmd_system_halt_id                                     0x0c010020
#define sl_bt_cmd_system_linklayer_configure_id                      0x0e010020
#define sl_bt_cmd_system_set_max_tx_power_id                         0x16010020
#define sl_bt_cmd_system_set_tx_power_id                             0x17010020
#define sl_bt_cmd_system_get_tx_power_setting_id                     0x18010020
#define sl_bt_cmd_system_set_identity_address_id                     0x13010020
#define sl_bt_cmd_system_get_identity_address_id                     0x15010020
#define sl_bt_cmd_system_get_random_data_id                          0x0b010020
#define sl_bt_cmd_system_data_buffer_write_id                        0x12010020
#define sl_bt_cmd_system_data_buffer_clear_id                        0x14010020
#define sl_bt_cmd_system_get_counters_id                             0x0f010020
#define sl_bt_cmd_system_set_soft_timer_id                           0x19010020
#define sl_bt_cmd_system_set_lazy_soft_timer_id                      0x1a010020
#define sl_bt_rsp_system_hello_id                                    0x00010020
#define sl_bt_rsp_system_start_bluetooth_id                          0x1c010020
#define sl_bt_rsp_system_stop_bluetooth_id                           0x1d010020
#define sl_bt_rsp_system_get_version_id                              0x1b010020
#define sl_bt_rsp_system_reset_id                                    0x01010020
#define sl_bt_rsp_system_halt_id                                     0x0c010020
#define sl_bt_rsp_system_linklayer_configure_id                      0x0e010020
#define sl_bt_rsp_system_set_max_tx_power_id                         0x16010020
#define sl_bt_rsp_system_set_tx_power_id                             0x17010020
#define sl_bt_rsp_system_get_tx_power_setting_id                     0x18010020
#define sl_bt_rsp_system_set_identity_address_id                     0x13010020
#define sl_bt_rsp_system_get_identity_address_id                     0x15010020
#define sl_bt_rsp_system_get_random_data_id                          0x0b010020
#define sl_bt_rsp_system_data_buffer_write_id                        0x12010020
#define sl_bt_rsp_system_data_buffer_clear_id                        0x14010020
#define sl_bt_rsp_system_get_counters_id                             0x0f010020
#define sl_bt_rsp_system_set_soft_timer_id                           0x19010020
#define sl_bt_rsp_system_set_lazy_soft_timer_id                      0x1a010020

/**
 * @brief Specifies the mode that the system will boot into
 */
typedef enum
{
  sl_bt_system_boot_mode_normal   = 0x0, /**< (0x0) Boot to normal mode */
  sl_bt_system_boot_mode_uart_dfu = 0x1, /**< (0x1) Boot to UART DFU mode */
  sl_bt_system_boot_mode_ota_dfu  = 0x2  /**< (0x2) Boot to OTA DFU mode */
} sl_bt_system_boot_mode_t;

/**
 * @brief These Keys are used to configure Link Layer Operation
 */
typedef enum
{
  sl_bt_system_linklayer_config_key_halt                       = 0x1,  /**<
                                                                            (0x1)
                                                                            Same
                                                                            as
                                                                            system_halt
                                                                            command,
                                                                            value-0
                                                                            Stop
                                                                            Radio
                                                                            1-
                                                                            Start
                                                                            Radio */
  sl_bt_system_linklayer_config_key_priority_range             = 0x2,  /**<
                                                                            (0x2)
                                                                            Sets
                                                                            the
                                                                            RAIL
                                                                            priority_mapping
                                                                            offset
                                                                            field
                                                                            of
                                                                            the
                                                                            link
                                                                            layer
                                                                            priority
                                                                            configuration
                                                                            structure
                                                                            to
                                                                            the
                                                                            first
                                                                            byte
                                                                            of
                                                                            the
                                                                            value
                                                                            field. */
  sl_bt_system_linklayer_config_key_scan_channels              = 0x3,  /**<
                                                                            (0x3)
                                                                            Sets
                                                                            channels
                                                                            to
                                                                            scan
                                                                            on.
                                                                            The
                                                                            first
                                                                            byte
                                                                            of
                                                                            the
                                                                            value
                                                                            is
                                                                            the
                                                                            channel
                                                                            map.
                                                                            0x1
                                                                            =
                                                                            Channel
                                                                            37,
                                                                            0x2
                                                                            =
                                                                            Channel
                                                                            38,
                                                                            0x4
                                                                            =
                                                                            Channel
                                                                            39 */
  sl_bt_system_linklayer_config_key_set_flags                  = 0x4,  /**<
                                                                            (0x4)
                                                                            Sets
                                                                            the
                                                                            link
                                                                            layer
                                                                            configuration
                                                                            flags.
                                                                            The
                                                                            value
                                                                            is a
                                                                            little
                                                                            endian
                                                                            32-bit
                                                                            integer.
                                                                            Flag
                                                                            Values:
                                                                              -
                                                                                0x00000001
                                                                                \-
                                                                                Disable
                                                                                Feature
                                                                                Exchange
                                                                                in
                                                                                peripheral
                                                                                role
                                                                                of
                                                                                the
                                                                                connection
                                                                              -
                                                                                0x00000002
                                                                                \-
                                                                                Disable
                                                                                Feature
                                                                                Exchange
                                                                                in
                                                                                central
                                                                                role
                                                                                of
                                                                                the
                                                                                connection */
  sl_bt_system_linklayer_config_key_clr_flags                  = 0x5,  /**<
                                                                            (0x5)
                                                                            The
                                                                            value
                                                                            is
                                                                            flags
                                                                            to
                                                                            clear.
                                                                            Flags
                                                                            are
                                                                            the
                                                                            same
                                                                            as
                                                                            in
                                                                            SET_FLAGS
                                                                            command. */
  sl_bt_system_linklayer_config_key_set_afh_interval           = 0x7,  /**<
                                                                            (0x7)
                                                                            Set
                                                                            afh_scan_interval
                                                                            field
                                                                            of
                                                                            Link
                                                                            Layer
                                                                            priority
                                                                            configuration
                                                                            structure. */
  sl_bt_system_linklayer_config_key_set_priority_table         = 0x9,  /**<
                                                                            (0x9)
                                                                            The
                                                                            value
                                                                            contains
                                                                            a
                                                                            priority
                                                                            table
                                                                            to
                                                                            be
                                                                            copied
                                                                            over
                                                                            the
                                                                            existing
                                                                            table.
                                                                            If
                                                                            the
                                                                            value
                                                                            is
                                                                            smaller
                                                                            than
                                                                            the
                                                                            full
                                                                            table,
                                                                            only
                                                                            those
                                                                            values
                                                                            are
                                                                            updated.
                                                                            See
                                                                            sl_bt_bluetooth_ll_priorities
                                                                            struct
                                                                            for
                                                                            the
                                                                            definition
                                                                            of a
                                                                            priority
                                                                            table. */
  sl_bt_system_linklayer_config_key_set_rx_packet_filtering    = 0xa,  /**<
                                                                            (0xa)
                                                                            Configure
                                                                            and
                                                                            enable
                                                                            or
                                                                            disable
                                                                            RX
                                                                            packet
                                                                            filtering
                                                                            feature.
                                                                            Value:
                                                                            >= 5
                                                                            bytes.
                                                                              -
                                                                                Byte
                                                                                1
                                                                                \-
                                                                                The
                                                                                filter
                                                                                count
                                                                              -
                                                                                Byte
                                                                                2
                                                                                \-
                                                                                The
                                                                                filter
                                                                                offset
                                                                              -
                                                                                Byte
                                                                                3
                                                                                \-
                                                                                The
                                                                                length
                                                                                of
                                                                                the
                                                                                filter
                                                                                list
                                                                              -
                                                                                Byte
                                                                                4
                                                                                \-
                                                                                The
                                                                                bitmask
                                                                                flags
                                                                              -
                                                                                Rest
                                                                                of
                                                                                the
                                                                                data
                                                                                \-
                                                                                The
                                                                                filter
                                                                                list */
  sl_bt_system_linklayer_config_key_set_simultaneous_scanning  = 0xb,  /**<
                                                                            (0xb)
                                                                            Enable
                                                                            or
                                                                            disable
                                                                            simultaneous
                                                                            scanning
                                                                            on
                                                                            the
                                                                            1M
                                                                            and
                                                                            Coded
                                                                            PHYs.
                                                                            Value:
                                                                            1
                                                                            byte.
                                                                              -
                                                                                0
                                                                                \-
                                                                                Disable
                                                                                simultaneous
                                                                                scanning.
                                                                              -
                                                                                1
                                                                                \-
                                                                                Enable
                                                                                simultaneous
                                                                                scanning. */
  sl_bt_system_linklayer_config_key_set_channelmap_flags       = 0xc,  /**<
                                                                            (0xc)
                                                                            Configure
                                                                            channelmap
                                                                            adaptivity
                                                                            flags.
                                                                            Value:
                                                                            4
                                                                            bytes. */
  sl_bt_system_linklayer_config_key_power_control_golden_range = 0x10  /**<
                                                                            (0x10)
                                                                            Power
                                                                            control
                                                                            golden
                                                                            range
                                                                            configuration.
                                                                            The
                                                                            first
                                                                            byte
                                                                            of
                                                                            the
                                                                            value
                                                                            is
                                                                            the
                                                                            lower
                                                                            boundary
                                                                            and
                                                                            the
                                                                            second
                                                                            byte
                                                                            is
                                                                            the
                                                                            upper
                                                                            boundary.
                                                                            Values
                                                                            are
                                                                            in
                                                                            dBm.
                                                                            Set
                                                                            golden
                                                                            range
                                                                            parameters.
                                                                            Value:
                                                                            8
                                                                            bytes.
                                                                              -
                                                                                Byte
                                                                                1
                                                                                \-
                                                                                Minimal
                                                                                RSSI
                                                                                on
                                                                                1M
                                                                                PHY
                                                                              -
                                                                                Byte
                                                                                2
                                                                                \-
                                                                                Maximal
                                                                                RSSI
                                                                                on
                                                                                1M
                                                                                PHY
                                                                              -
                                                                                Byte
                                                                                3
                                                                                \-
                                                                                Minimal
                                                                                RSSI
                                                                                on
                                                                                2M
                                                                                PHY
                                                                              -
                                                                                Byte
                                                                                4
                                                                                \-
                                                                                Maximal
                                                                                RSSI
                                                                                on
                                                                                2M
                                                                                PHY
                                                                              -
                                                                                Byte
                                                                                5
                                                                                \-
                                                                                Minimal
                                                                                RSSI
                                                                                on
                                                                                Coded
                                                                                PHY
                                                                                S=8
                                                                              -
                                                                                Byte
                                                                                6
                                                                                \-
                                                                                Maximal
                                                                                RSSI
                                                                                on
                                                                                Coded
                                                                                PHY
                                                                                S=8
                                                                              -
                                                                                Byte
                                                                                7
                                                                                \-
                                                                                Minimal
                                                                                RSSI
                                                                                on
                                                                                Coded
                                                                                PHY
                                                                                S=2
                                                                              -
                                                                                Byte
                                                                                8
                                                                                \-
                                                                                Maximal
                                                                                RSSI
                                                                                on
                                                                                Coded
                                                                                PHY
                                                                                S=2 */
} sl_bt_system_linklayer_config_key_t;

/**
 * @addtogroup sl_bt_evt_system_boot sl_bt_evt_system_boot
 * @{
 * @brief Indicates that the device has started and the radio is ready
 *
 * This event carries the firmware build number and other software and hardware
 * identification codes.
 */

/** @brief Identifier of the boot event */
#define sl_bt_evt_system_boot_id                                     0x000100a0

/***************************************************************************//**
 * @brief Data structure of the boot event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_system_boot_s
{
  uint16_t major;      /**< Major release version */
  uint16_t minor;      /**< Minor release version */
  uint16_t patch;      /**< Patch release number */
  uint16_t build;      /**< Build number */
  uint32_t bootloader; /**< Bootloader version */
  uint16_t hw;         /**< Hardware type */
  uint32_t hash;       /**< Version hash */
});

typedef struct sl_bt_evt_system_boot_s sl_bt_evt_system_boot_t;

/** @} */ // end addtogroup sl_bt_evt_system_boot

/**
 * @addtogroup sl_bt_evt_system_error sl_bt_evt_system_error
 * @{
 * @brief Indicates that an error has occurred
 *
 * See error codes table for more information.
 */

/** @brief Identifier of the error event */
#define sl_bt_evt_system_error_id                                    0x060100a0

/***************************************************************************//**
 * @brief Data structure of the error event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_system_error_s
{
  uint16_t   reason; /**< Result code
                            - <b>0:</b> success
                            - <b>Non-zero:</b> an error has occurred */
  uint8array data;   /**< Data related to the error; this field can be empty. */
});

typedef struct sl_bt_evt_system_error_s sl_bt_evt_system_error_t;

/** @} */ // end addtogroup sl_bt_evt_system_error

/**
 * @addtogroup sl_bt_evt_system_hardware_error sl_bt_evt_system_hardware_error
 * @{
 * @brief Indicates that a hardware-related error has occurred.
 */

/** @brief Identifier of the hardware_error event */
#define sl_bt_evt_system_hardware_error_id                           0x050100a0

/***************************************************************************//**
 * @brief Data structure of the hardware_error event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_system_hardware_error_s
{
  uint16_t status; /**< Result code
                          - <b>0:</b> success
                          - <b>Non-zero:</b> an error has occurred */
});

typedef struct sl_bt_evt_system_hardware_error_s sl_bt_evt_system_hardware_error_t;

/** @} */ // end addtogroup sl_bt_evt_system_hardware_error

/**
 * @addtogroup sl_bt_evt_system_external_signal sl_bt_evt_system_external_signal
 * @{
 * @brief Indicates that the external signals have been received
 *
 * External signals are generated from the native application.
 */

/** @brief Identifier of the external_signal event */
#define sl_bt_evt_system_external_signal_id                          0x030100a0

/***************************************************************************//**
 * @brief Data structure of the external_signal event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_system_external_signal_s
{
  uint32_t extsignals; /**< Bitmask of external signals received since last
                            event. */
});

typedef struct sl_bt_evt_system_external_signal_s sl_bt_evt_system_external_signal_t;

/** @} */ // end addtogroup sl_bt_evt_system_external_signal

/**
 * @addtogroup sl_bt_evt_system_awake sl_bt_evt_system_awake
 * @{
 * @brief Indicates that the device is awake and no longer in sleep mode
 *
 * <b>NOTE:</b> Stack does not generate this event by itself because sleep and
 * wakeup are managed by applications. If this event is needed, call function
 * @ref sl_bt_send_system_awake, which signals the stack to send the event.
 */

/** @brief Identifier of the awake event */
#define sl_bt_evt_system_awake_id                                    0x040100a0

/** @} */ // end addtogroup sl_bt_evt_system_awake

/**
 * @addtogroup sl_bt_evt_system_soft_timer sl_bt_evt_system_soft_timer
 * @{
 * @brief Indicates that a soft timer has lapsed.
 */

/** @brief Identifier of the soft_timer event */
#define sl_bt_evt_system_soft_timer_id                               0x070100a0

/***************************************************************************//**
 * @brief Data structure of the soft_timer event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_system_soft_timer_s
{
  uint8_t handle; /**< Timer Handle */
});

typedef struct sl_bt_evt_system_soft_timer_s sl_bt_evt_system_soft_timer_t;

/** @} */ // end addtogroup sl_bt_evt_system_soft_timer

/***************************************************************************//**
 *
 * Verify whether the communication between the host and the device is
 * functional.
 *
 * <b>NOTE:</b> This command is available even if the Bluetooth stack has not
 * been started. See @ref sl_bt_system_start_bluetooth for description of how
 * the Bluetooth stack is started.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_hello();

/***************************************************************************//**
 *
 * If the Bluetooth on-demand start component is not included in the application
 * build, the Bluetooth stack is automatically started at UC initialization
 * time. In this configuration the on-demand start command is not available and
 * the command returns the error SL_STATUS_NOT_AVAILABLE.
 *
 * When the Bluetooth on-demand start component is included in the application
 * build, this command is used by the application to request starting the
 * Bluetooth stack when the application needs it. If the command returns a
 * success result, the stack starts to asynchronously allocate the resources and
 * configure the Bluetooth stack based on the configuration passed at UC
 * initialization time.
 *
 * Successful start of the stack is indicated by the @ref sl_bt_evt_system_boot
 * event. The configured classes and Bluetooth stack features are available
 * after the application has received the @ref sl_bt_evt_system_boot event. If
 * starting the Bluetooth stack fails, the error is indicated to the application
 * with the @ref sl_bt_evt_system_error event.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_start_bluetooth();

/***************************************************************************//**
 *
 * If the Bluetooth on-demand start component is not included in the application
 * build, the Bluetooth stack is automatically started at UC initialization time
 * and never stopped. In this configuration the stop command is not available
 * and the command returns the error SL_STATUS_NOT_AVAILABLE.
 *
 * When the Bluetooth on-demand start component is included in the application
 * build, this command is used by the application to stop the Bluetooth stack
 * when the application no longer needs it. This command gracefully restores
 * Bluetooth to an idle state by disconnecting any active connections and
 * stopping any on-going advertising and scanning. Any resources that were
 * allocated when the stack was started are freed when the stack is stopped.
 * After this command the BGAPI classes other than @ref sl_bt_system become
 * unavailable. The application can use the command @ref
 * sl_bt_system_start_bluetooth in order to continue using Bluetooth later.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_stop_bluetooth();

/***************************************************************************//**
 *
 * Get the firmware version information.
 *
 * <b>NOTE:</b> This command is available even if the Bluetooth stack has not
 * been started. See @ref sl_bt_system_start_bluetooth for description of how
 * the Bluetooth stack is started.
 *
 * @param[out] major Major release version
 * @param[out] minor Minor release version
 * @param[out] patch Patch release number
 * @param[out] build Build number
 * @param[out] bootloader Bootloader version
 * @param[out] hash Version hash
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_get_version(uint16_t *major,
                                     uint16_t *minor,
                                     uint16_t *patch,
                                     uint16_t *build,
                                     uint32_t *bootloader,
                                     uint32_t *hash);

/***************************************************************************//**
 *
 * Reset the system. The command does not have a response but it triggers one of
 * the boot events (normal reset or boot to DFU mode) depending on the selected
 * boot mode.
 *
 * <b>NOTE:</b> This command is available even if the Bluetooth stack has not
 * been started. See @ref sl_bt_system_start_bluetooth for description of how
 * the Bluetooth stack is started.
 *
 * @param[in] dfu Enum @ref sl_bt_system_boot_mode_t. Boot mode. Values:
 *     - <b>sl_bt_system_boot_mode_normal (0x0):</b> Boot to normal mode
 *     - <b>sl_bt_system_boot_mode_uart_dfu (0x1):</b> Boot to UART DFU mode
 *     - <b>sl_bt_system_boot_mode_ota_dfu (0x2):</b> Boot to OTA DFU mode
 *
 * @b Events
 *   - @ref sl_bt_evt_system_boot - Sent after the device has booted in normal
 *     mode.
 *   - @ref sl_bt_evt_dfu_boot - Sent after the device has booted in UART DFU
 *     mode.
 *
 ******************************************************************************/
void sl_bt_system_reset(uint8_t dfu);

/***************************************************************************//**
 *
 * Force radio to idle state and allow device to sleep. Advertising, scanning,
 * connections, and software timers are halted by this command. Halted
 * operations resume after calling this command with parameter 0. Connections
 * stay alive if the system is resumed before connection supervision timeout.
 *
 * Use this command only for a short time period (a few seconds at maximum).
 * Although it halts Bluetooth activity, all tasks and operations still exist
 * inside the stack with their own concepts of time. Halting the system for a
 * long time period may have negative consequences on stack's internal states.
 *
 * <b>NOTE:</b> The software timer is also halted. Hardware interrupts are the
 * only way to wake up from energy mode 2 when the system is halted.
 *
 * @param[in] halt Values:
 *     - <b>1:</b> halt
 *     - <b>0:</b> resume
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_halt(uint8_t halt);

/***************************************************************************//**
 *
 * Send configuration data to the link layer. This command fine tunes low-level
 * Bluetooth operations.
 *
 * @param[in] key @parblock
 *   Enum @ref sl_bt_system_linklayer_config_key_t. Key to configure. Values:
 *     - <b>sl_bt_system_linklayer_config_key_halt (0x1):</b> Same as
 *       system_halt command, value-0 Stop Radio 1- Start Radio
 *     - <b>sl_bt_system_linklayer_config_key_priority_range (0x2):</b> Sets the
 *       RAIL priority_mapping offset field of the link layer priority
 *       configuration structure to the first byte of the value field.
 *     - <b>sl_bt_system_linklayer_config_key_scan_channels (0x3):</b> Sets
 *       channels to scan on. The first byte of the value is the channel map.
 *       0x1 = Channel 37, 0x2 = Channel 38, 0x4 = Channel 39
 *     - <b>sl_bt_system_linklayer_config_key_set_flags (0x4):</b> Sets the link
 *       layer configuration flags. The value is a little endian 32-bit integer.
 *       Flag Values:
 *         - 0x00000001 - Disable Feature Exchange in peripheral role of the
 *           connection
 *         - 0x00000002 - Disable Feature Exchange in central role of the
 *           connection
 *
 *     - <b>sl_bt_system_linklayer_config_key_clr_flags (0x5):</b> The value is
 *       flags to clear. Flags are the same as in SET_FLAGS command.
 *     - <b>sl_bt_system_linklayer_config_key_set_afh_interval (0x7):</b> Set
 *       afh_scan_interval field of Link Layer priority configuration structure.
 *     - <b>sl_bt_system_linklayer_config_key_set_priority_table (0x9):</b> The
 *       value contains a priority table to be copied over the existing table.
 *       If the value is smaller than the full table, only those values are
 *       updated. See sl_bt_bluetooth_ll_priorities struct for the definition of
 *       a priority table.
 *     - <b>sl_bt_system_linklayer_config_key_set_rx_packet_filtering (0xa):</b>
 *       Configure and enable or disable RX packet filtering feature. Value: >=
 *       5 bytes.
 *         - Byte 1 - The filter count
 *         - Byte 2 - The filter offset
 *         - Byte 3 - The length of the filter list
 *         - Byte 4 - The bitmask flags
 *         - Rest of the data - The filter list
 *
 *     - <b>sl_bt_system_linklayer_config_key_set_simultaneous_scanning
 *       (0xb):</b> Enable or disable simultaneous scanning on the 1M and Coded
 *       PHYs. Value: 1 byte.
 *         - 0 - Disable simultaneous scanning.
 *         - 1 - Enable simultaneous scanning.
 *
 *     - <b>sl_bt_system_linklayer_config_key_set_channelmap_flags (0xc):</b>
 *       Configure channelmap adaptivity flags. Value: 4 bytes.
 *     - <b>sl_bt_system_linklayer_config_key_power_control_golden_range
 *       (0x10):</b> Power control golden range configuration. The first byte of
 *       the value is the lower boundary and the second byte is the upper
 *       boundary. Values are in dBm. Set golden range parameters. Value: 8
 *       bytes.
 *         - Byte 1 - Minimal RSSI on 1M PHY
 *         - Byte 2 - Maximal RSSI on 1M PHY
 *         - Byte 3 - Minimal RSSI on 2M PHY
 *         - Byte 4 - Maximal RSSI on 2M PHY
 *         - Byte 5 - Minimal RSSI on Coded PHY S=8
 *         - Byte 6 - Maximal RSSI on Coded PHY S=8
 *         - Byte 7 - Minimal RSSI on Coded PHY S=2
 *         - Byte 8 - Maximal RSSI on Coded PHY S=2
 *   @endparblock
 * @param[in] data_len Length of data in @p data
 * @param[in] data Configuration data. Length and contents of the data field
 *   depend on the key value used.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_linklayer_configure(uint8_t key,
                                             size_t data_len,
                                             const uint8_t* data);

/***************************************************************************//**
 *
 * <b>Deprecated</b> and replaced by @ref sl_bt_system_set_tx_power.
 *
 * Set the global maximum radiated TX power for Bluetooth. This returns the
 * selected power level that is radiated from the antenna at TX. The transmitter
 * power at antenna pin will apply the RF TX path gain to match this setting. RF
 * TX path gain can be set in the Bluetooth configuration. If the GATT server
 * contains a TX power service, the TX Power Level attribute will be updated
 * with the selected maximum power level.
 *
 * The selected power level may be less than the specified value if the device
 * does not meet the power requirements. For Bluetooth connections, the maximum
 * TX power is limited to 10 dBm if Adaptive Frequency Hopping (AFH) is not
 * enabled.
 *
 * The maximum TX power level can also be configured in the Bluetooth
 * configuration and passed into the Bluetooth stack initialization. By default,
 * the global maximum TX power is 8 dBm.
 *
 * <b>NOTE:</b> Do not use this command while advertising or scanning.
 * Furthermore, the stack does not allow setting TX powers during connections.
 *
 * @param[in] power The maximum radiated TX power in 0.1 dBm steps. For example,
 *   value 10 means 1 dBm.
 * @param[out] set_power Selected maximum radiated TX power
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
SL_BGAPI_DEPRECATED sl_status_t sl_bt_system_set_max_tx_power(int16_t power, int16_t *set_power);

/***************************************************************************//**
 *
 * Set the global minimum and maximum radiated TX power levels for Bluetooth.
 * This returns selected power levels that are radiated from the antenna at TX.
 * The transmitter power at antenna pin will apply the RF TX path gain to match
 * this setting. RF TX path gain can be set in the Bluetooth configuration. If
 * the GATT server contains a TX power service, the TX Power Level attribute
 * will be updated with the selected maximum power level.
 *
 * A selected power level may be different than the requested value if the
 * device cannot meet the requirement. For Bluetooth connections, the maximum
 * radiated TX power is limited to 10 dBm if Adaptive Frequency Hopping (AFH) is
 * not enabled.
 *
 * The minimum TX power setting is used by LE power control. It has no effect in
 * Bluetooth stack if the LE power control feature is not enabled. However, the
 * application may still use this setting for other purposes, e.g., setting the
 * minimum TX power for DTM transmitter test.
 *
 * The minimum and maximum radiated TX power levels can also be configured in
 * the Bluetooth configuration and passed into the Bluetooth stack
 * initialization. By default, the minimum radiated TX power level is configured
 * to -3 dBm and the maximum radiated TX power level to 8 dBm.
 *
 * <b>NOTE:</b> Do not use this command while advertising or scanning.
 * Furthermore, the stack does not allow setting TX powers during connections.
 *
 * @param[in] min_power Minimum radiated TX power. Unit: 0.1 dBm. For example,
 *   the value 10 means 1 dBm.
 * @param[in] max_power Maximum radiated TX power. Unit: 0.1 dBm. For example,
 *   the value 10 means 1 dBm.
 * @param[out] set_min The selected minimum radiated TX power. Unit: 0.1 dBm
 * @param[out] set_max The selected maximum radiated TX power. Unit: 0.1 dBm
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_set_tx_power(int16_t min_power,
                                      int16_t max_power,
                                      int16_t *set_min,
                                      int16_t *set_max);

/***************************************************************************//**
 *
 * Get TX power settings including the minimum and maximum radiated TX power
 * levels the device supports, the minimum and maximum radiated TX power levels
 * currently set in the stack, and the TX RF path gain configuration.
 *
 * @param[out] support_min The minimum radiated TX power the device supports.
 *   Unit: 0.1 dBm
 * @param[out] support_max The maximum radiated TX power the device supports.
 *   Unit: 0.1 dBm
 * @param[out] set_min The minimum radiated TX power currently set in stack.
 *   Unit: 0.1 dBm
 * @param[out] set_max The maximum radiated TX power currently set in stack.
 *   Unit: 0.1 dBm
 * @param[out] rf_path_gain TX RF path gain. Unit: 0.1 dBm
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_get_tx_power_setting(int16_t *support_min,
                                              int16_t *support_max,
                                              int16_t *set_min,
                                              int16_t *set_max,
                                              int16_t *rf_path_gain);

/***************************************************************************//**
 *
 * Set the device's Bluetooth identity address. The address can be a public
 * device address or a static device address. A valid address set with this
 * command will be written into persistent storage using NVM keys. The stack
 * returns an error if the static device address does not conform to the
 * Bluetooth specification.
 *
 * The new address will be effective in the next system reboot. The stack will
 * use the address in the NVM keys when present. Otherwise, it uses the default
 * Bluetooth public device address which is programmed at production.
 *
 * The stack treats 00:00:00:00:00:00 and ff:ff:ff:ff:ff:ff as invalid
 * addresses. Therefore, passing one of them into this command will cause the
 * stack to delete the NVM keys and use the default address in the next system
 * reboot.
 *
 * <b>Note:</b> Because the NVM keys are located in flash and flash wearing can
 * occur, avoid calling this command regularly.
 *
 * @param[in] address Bluetooth identity address in little endian format
 * @param[in] type Address type
 *     - <b>0:</b> Public device address
 *     - <b>1:</b> Static device address
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_set_identity_address(bd_addr address, uint8_t type);

/***************************************************************************//**
 *
 * Read the Bluetooth identity address used by the device, which can be a public
 * or random static device address.
 *
 * @param[out] address Bluetooth public address in little endian format
 * @param[out] type Address type
 *     - <b>0:</b> Public device address
 *     - <b>1:</b> Static random address
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_get_identity_address(bd_addr *address, uint8_t *type);

/***************************************************************************//**
 *
 * Get random data.
 *
 * @param[in] length Length of random data.
 * @param[in] max_data_size Size of output buffer passed in @p data
 * @param[out] data_len On return, set to the length of output data written to
 *   @p data
 * @param[out] data Random data
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_get_random_data(uint8_t length,
                                         size_t max_data_size,
                                         size_t *data_len,
                                         uint8_t *data);

/***************************************************************************//**
 *
 * Write data into the system data buffer. Data will be appended to the end of
 * existing data.
 *
 * @param[in] data_len Length of data in @p data
 * @param[in] data Data to write
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_data_buffer_write(size_t data_len,
                                           const uint8_t* data);

/***************************************************************************//**
 *
 * Remove all data from the system data buffer.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_data_buffer_clear();

/***************************************************************************//**
 *
 * Get packet and error counters. Passing a non-zero value also resets counters.
 *
 * @param[in] reset Reset counters if the parameter value is not zero.
 * @param[out] tx_packets The number of successfully transmitted packets
 * @param[out] rx_packets The number of successfully received packets
 * @param[out] crc_errors The number of received packets with CRC errors
 * @param[out] failures The number of radio failures, such as aborted TX/RX
 *   packets, scheduling failures, and so on.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_get_counters(uint8_t reset,
                                      uint16_t *tx_packets,
                                      uint16_t *rx_packets,
                                      uint16_t *crc_errors,
                                      uint16_t *failures);

/***************************************************************************//**
 *
 * <b>Deprecated</b> . Use the sleeptimer component (in platform services
 * category) for timers.
 *
 * Start a software timer. Multiple concurrent timers can be running
 * simultaneously. 256 unique timer handles (IDs) are available. The maximum
 * number of concurrent timers is configurable at device initialization. Up to
 * 16 concurrent timers can be configured. The default configuration is 4. As
 * the RAM for storing timer data is pre-allocated at initialization, an
 * application should not configure the amount more than it needs for minimizing
 * RAM usage.
 *
 * @param[in] time @parblock
 *   Frequency interval of events, which indicates how often to send events in
 *   hardware clock ticks (1 second is equal to 32768 ticks).
 *
 *   The smallest supported interval value is 328, which is around 10
 *   milliseconds. Any parameters between 0 and 328 will be rounded up to 328.
 *   The maximum value is 2147483647, which corresponds to about 18.2 hours.
 *
 *   If @p time is 0, removes the scheduled timer with the same handle.
 *   @endparblock
 * @param[in] handle Timer handle to use, which is returned in timeout event
 * @param[in] single_shot Timer mode. Values:
 *     - <b>0:</b> false (timer is repeating)
 *     - <b>1:</b> true (timer runs only once)
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_system_soft_timer - Sent after this timer has lapsed.
 *
 ******************************************************************************/
SL_BGAPI_DEPRECATED sl_status_t sl_bt_system_set_soft_timer(uint32_t time,
                                        uint8_t handle,
                                        uint8_t single_shot);

/***************************************************************************//**
 *
 * <b>Deprecated</b> . Use the sleeptimer component (in platform services
 * category) for timers. As the sleeptimer does not support a timer with slack
 * yet, the Bluetooth stack will continue to support this command until another
 * component provides the functionality.
 *
 * Start a software timer with slack. The slack parameter allows the stack to
 * optimize wakeups and save power. The timer event is triggered between time
 * and time + @p slack. See also description of @ref sl_bt_system_set_soft_timer
 * command.
 *
 * @param[in] time @parblock
 *   An interval between how often to send events in hardware clock ticks (1
 *   second is equal to 32768 ticks).
 *
 *   The smallest interval value supported is 328, which is around 10
 *   milliseconds. Any parameters between 0 and 328 will be rounded up to 328.
 *   The maximum value is 2147483647, which corresponds to about 18.2 hours.
 *
 *   If @p time is 0, removes the scheduled timer with the same handle.
 *   @endparblock
 * @param[in] slack Slack time in hardware clock ticks
 * @param[in] handle Timer handle to use, which is returned in timeout event
 * @param[in] single_shot Timer mode. Values:
 *     - <b>0:</b> false (timer is repeating)
 *     - <b>1:</b> true (timer runs only once)
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_system_soft_timer - Sent after this timer has lapsed.
 *
 ******************************************************************************/
SL_BGAPI_DEPRECATED sl_status_t sl_bt_system_set_lazy_soft_timer(uint32_t time,
                                             uint32_t slack,
                                             uint8_t handle,
                                             uint8_t single_shot);

/** @} */ // end addtogroup sl_bt_system

/**
 * @addtogroup sl_bt_gap GAP
 * @{
 *
 * @brief GAP
 *
 * The commands and events in this class are related to the Generic Access
 * Profile (GAP) in Bluetooth.
 */

/* Command and Response IDs */
#define sl_bt_cmd_gap_set_privacy_mode_id                            0x01020020
#define sl_bt_cmd_gap_set_data_channel_classification_id             0x02020020
#define sl_bt_cmd_gap_enable_whitelisting_id                         0x03020020
#define sl_bt_rsp_gap_set_privacy_mode_id                            0x01020020
#define sl_bt_rsp_gap_set_data_channel_classification_id             0x02020020
#define sl_bt_rsp_gap_enable_whitelisting_id                         0x03020020

/**
 * @brief These values define Bluetooth device address types.
 */
typedef enum
{
  sl_bt_gap_public_address               = 0x0, /**< (0x0) Public device address */
  sl_bt_gap_static_address               = 0x1, /**< (0x1) Static device address */
  sl_bt_gap_random_resolvable_address    = 0x2, /**< (0x2) Resolvable private
                                                     random address */
  sl_bt_gap_random_nonresolvable_address = 0x3  /**< (0x3) Non-resolvable
                                                     private random address */
} sl_bt_gap_address_type_t;

/**
 * @brief Types of PHYs
 */
typedef enum
{
  sl_bt_gap_phy_1m    = 0x1,  /**< (0x1) 1M PHY */
  sl_bt_gap_phy_2m    = 0x2,  /**< (0x2) 2M PHY */
  sl_bt_gap_phy_coded = 0x4,  /**< (0x4) Coded PHY, 125k (S=8) or 500k (S=2) */
  sl_bt_gap_phy_any   = 0xff  /**< (0xff) Any PHYs the device supports */
} sl_bt_gap_phy_t;

/**
 * @brief PHY types with coding schemes
 */
typedef enum
{
  sl_bt_gap_phy_coding_1m_uncoded = 0x1, /**< (0x1) 1M PHY */
  sl_bt_gap_phy_coding_2m_uncoded = 0x2, /**< (0x2) 2M PHY */
  sl_bt_gap_phy_coding_125k_coded = 0x4, /**< (0x4) 125k Coded PHY (S=8) */
  sl_bt_gap_phy_coding_500k_coded = 0x8  /**< (0x8) 500k Coded PHY (S=2) */
} sl_bt_gap_phy_coding_t;

/***************************************************************************//**
 *
 * Enable or disable the privacy feature on all GAP roles. New privacy mode will
 * take effect for advertising next time advertising is enabled, for scanning
 * next time scanning is enabled, and for initiating on the next open connection
 * command.
 *
 * When privacy is enabled and the device is advertising or scanning, the stack
 * will maintain a periodic timer with the specified time interval as a timeout
 * value. At each timeout, the stack will generate a new resolvable private
 * address and use it in scanning requests. For advertisers, the stack will
 * generate a new resolvable or non-resolvable private address and use it in
 * advertising data packets for each advertising set if its address is not
 * application-managed, i.e., the address was not set by the application (with
 * the @ref sl_bt_advertiser_set_random_address command). The application is
 * fully responsible for application-managed advertiser addresses. For an
 * application-managed resolvable private address, the application should
 * schedule periodic address updates for enhancing the privacy. It is
 * recommended to use different schedules for different advertising sets.
 *
 * Disabling the privacy during active advertising or scanning is not allowed.
 *
 * By default, privacy feature is disabled.
 *
 * @param[in] privacy Values:
 *     - <b>0:</b> Disable privacy
 *     - <b>1:</b> Enable privacy
 * @param[in] interval The minimum time interval between a private address
 *   change. This parameter is ignored if this command is issued to disable
 *   privacy mode. Values:
 *     - <b>0:</b> Use default interval, 15 minutes
 *     - <b>others:</b> The time interval in minutes
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gap_set_privacy_mode(uint8_t privacy, uint8_t interval);

/***************************************************************************//**
 *
 * Specify a channel classification for data channels. This classification
 * persists until overwritten with a subsequent command or until the system is
 * reset.
 *
 * @param[in] channel_map_len Length of data in @p channel_map
 * @param[in] channel_map @parblock
 *   This parameter is 5 bytes and contains 37 1-bit fields.
 *   The nth field (in the range 0 to 36) contains the value for the link layer
 *   channel index n.
 *     - <b>0:</b> Channel n is bad.
 *     - <b>1:</b> Channel n is unknown.
 *
 *   The rest of most significant bits are reserved for future use and must be
 *   set to 0.
 *   At least two channels shall be marked as unknown.
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gap_set_data_channel_classification(size_t channel_map_len,
                                                      const uint8_t* channel_map);

/***************************************************************************//**
 *
 * Enable or disable accept list filtering. The setting will be effective the
 * next time that scanning is enabled. To add devices to the accept list, either
 * bond with the device or add it manually with @ref sl_bt_sm_add_to_whitelist.
 *
 * @param[in] enable 1 enable, 0 disable accept list filtering.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gap_enable_whitelisting(uint8_t enable);

/** @} */ // end addtogroup sl_bt_gap

/**
 * @addtogroup sl_bt_advertiser Advertiser
 * @{
 *
 * @brief Advertiser
 *
 * The commands and events in this class are related to advertising
 * functionalities in GAP peripheral and broadcaster roles.
 */

/* Command and Response IDs */
#define sl_bt_cmd_advertiser_create_set_id                           0x01040020
#define sl_bt_cmd_advertiser_set_timing_id                           0x03040020
#define sl_bt_cmd_advertiser_set_phy_id                              0x06040020
#define sl_bt_cmd_advertiser_set_channel_map_id                      0x04040020
#define sl_bt_cmd_advertiser_set_tx_power_id                         0x0b040020
#define sl_bt_cmd_advertiser_set_report_scan_request_id              0x05040020
#define sl_bt_cmd_advertiser_set_random_address_id                   0x10040020
#define sl_bt_cmd_advertiser_clear_random_address_id                 0x11040020
#define sl_bt_cmd_advertiser_set_configuration_id                    0x07040020
#define sl_bt_cmd_advertiser_clear_configuration_id                  0x08040020
#define sl_bt_cmd_advertiser_set_data_id                             0x0f040020
#define sl_bt_cmd_advertiser_set_long_data_id                        0x0e040020
#define sl_bt_cmd_advertiser_start_id                                0x09040020
#define sl_bt_cmd_advertiser_stop_id                                 0x0a040020
#define sl_bt_cmd_advertiser_start_periodic_advertising_id           0x0c040020
#define sl_bt_cmd_advertiser_stop_periodic_advertising_id            0x0d040020
#define sl_bt_cmd_advertiser_delete_set_id                           0x02040020
#define sl_bt_rsp_advertiser_create_set_id                           0x01040020
#define sl_bt_rsp_advertiser_set_timing_id                           0x03040020
#define sl_bt_rsp_advertiser_set_phy_id                              0x06040020
#define sl_bt_rsp_advertiser_set_channel_map_id                      0x04040020
#define sl_bt_rsp_advertiser_set_tx_power_id                         0x0b040020
#define sl_bt_rsp_advertiser_set_report_scan_request_id              0x05040020
#define sl_bt_rsp_advertiser_set_random_address_id                   0x10040020
#define sl_bt_rsp_advertiser_clear_random_address_id                 0x11040020
#define sl_bt_rsp_advertiser_set_configuration_id                    0x07040020
#define sl_bt_rsp_advertiser_clear_configuration_id                  0x08040020
#define sl_bt_rsp_advertiser_set_data_id                             0x0f040020
#define sl_bt_rsp_advertiser_set_long_data_id                        0x0e040020
#define sl_bt_rsp_advertiser_start_id                                0x09040020
#define sl_bt_rsp_advertiser_stop_id                                 0x0a040020
#define sl_bt_rsp_advertiser_start_periodic_advertising_id           0x0c040020
#define sl_bt_rsp_advertiser_stop_periodic_advertising_id            0x0d040020
#define sl_bt_rsp_advertiser_delete_set_id                           0x02040020

/**
 * @brief 
                These values define the available connectable modes, which indicate whether the device accepts
                connection requests or scan requests.
            
 */
typedef enum
{
  sl_bt_advertiser_non_connectable           = 0x0, /**< (0x0) Non-connectable
                                                         non-scannable. */
  sl_bt_advertiser_directed_connectable      = 0x1, /**< (0x1) Directed
                                                         connectable
                                                         <b>(RESERVED, DO NOT
                                                         USE)</b> */
  sl_bt_advertiser_connectable_scannable     = 0x2, /**< (0x2) Undirected
                                                         connectable scannable.
                                                         This mode can only be
                                                         used in legacy
                                                         advertising PDUs. */
  sl_bt_advertiser_scannable_non_connectable = 0x3, /**< (0x3) Undirected
                                                         scannable
                                                         (Non-connectable but
                                                         responds to scan
                                                         requests) */
  sl_bt_advertiser_connectable_non_scannable = 0x4  /**< (0x4) Undirected
                                                         connectable
                                                         non-scannable. This
                                                         mode can only be used
                                                         in extended advertising
                                                         PDUs. */
} sl_bt_advertiser_connectable_mode_t;

/**
 * @brief 
                These values define the available Discoverable Modes, which dictate how the device is visible to other
                devices.
            
 */
typedef enum
{
  sl_bt_advertiser_non_discoverable     = 0x0, /**< (0x0) Not discoverable */
  sl_bt_advertiser_limited_discoverable = 0x1, /**< (0x1) Discoverable using
                                                    both limited and general
                                                    discovery procedures */
  sl_bt_advertiser_general_discoverable = 0x2, /**< (0x2) Discoverable using
                                                    general discovery procedure */
  sl_bt_advertiser_broadcast            = 0x3, /**< (0x3) Device is not
                                                    discoverable in either
                                                    limited or generic discovery
                                                    procedure but may be
                                                    discovered using the
                                                    Observation procedure. */
  sl_bt_advertiser_user_data            = 0x4  /**< (0x4) Send advertising
                                                    and/or scan response data
                                                    defined by the user. The
                                                    limited/general discoverable
                                                    flags are defined by the
                                                    user. */
} sl_bt_advertiser_discoverable_mode_t;

/**
 * @brief Address type to use for advertising
 */
typedef enum
{
  sl_bt_advertiser_identity_address = 0x0, /**< (0x0) Use public or static
                                                device address, or an identity
                                                address if privacy mode is
                                                enabled. */
  sl_bt_advertiser_non_resolvable   = 0x1  /**< (0x1) Use non resolvable address
                                                type; advertising mode must also
                                                be non-connectable. */
} sl_bt_advertiser_adv_address_type_t;

/**
 * @addtogroup sl_bt_evt_advertiser_timeout sl_bt_evt_advertiser_timeout
 * @{
 * @brief Indicates the advertising of an advertising set has stopped, because
 * the advertiser has completed the configured number of advertising events or
 * the advertising has reached the configured duration
 *
 * The maximum number of advertising events or advertising duration can be
 * configured by the @p maxevents or @p duration parameter in the command @ref
 * sl_bt_advertiser_set_timing.
 */

/** @brief Identifier of the timeout event */
#define sl_bt_evt_advertiser_timeout_id                              0x010400a0

/***************************************************************************//**
 * @brief Data structure of the timeout event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_advertiser_timeout_s
{
  uint8_t handle; /**< The advertising set handle */
});

typedef struct sl_bt_evt_advertiser_timeout_s sl_bt_evt_advertiser_timeout_t;

/** @} */ // end addtogroup sl_bt_evt_advertiser_timeout

/**
 * @addtogroup sl_bt_evt_advertiser_scan_request sl_bt_evt_advertiser_scan_request
 * @{
 * @brief Reports any scan request received in advertising mode if the scan
 * request notification is enabled
 *
 * Do not confuse this event with the scan response.
 */

/** @brief Identifier of the scan_request event */
#define sl_bt_evt_advertiser_scan_request_id                         0x020400a0

/***************************************************************************//**
 * @brief Data structure of the scan_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_advertiser_scan_request_s
{
  uint8_t handle;       /**< Advertising set handle where scan request was
                             received */
  bd_addr address;      /**< Bluetooth address of the scanning device */
  uint8_t address_type; /**< Scanner address type. Values:
                               - <b>0:</b> Public address
                               - <b>1:</b> Random address */
  uint8_t bonding;      /**< Bonding handle if the remote scanning device has
                             previously bonded with the local device. Values:
                               - <b>SL_BT_INVALID_BONDING_HANDLE (0xff):</b> No
                                 bonding
                               - <b>Other:</b> Bonding handle */
});

typedef struct sl_bt_evt_advertiser_scan_request_s sl_bt_evt_advertiser_scan_request_t;

/** @} */ // end addtogroup sl_bt_evt_advertiser_scan_request

/**
 * @cond RESTRICTED
 *
 * @addtogroup sl_bt_evt_advertiser_periodic_advertising_status sl_bt_evt_advertiser_periodic_advertising_status
 * @{
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief This event indicates a status update in the periodic advertising.
 */

/** @brief Identifier of the periodic_advertising_status event */
#define sl_bt_evt_advertiser_periodic_advertising_status_id          0x030400a0

/***************************************************************************//**
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Data structure of the periodic_advertising_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_advertiser_periodic_advertising_status_s
{
  uint8_t  sid;    /**< The advertising set handle */
  uint32_t status; /**< Reserved for future */
});

typedef struct sl_bt_evt_advertiser_periodic_advertising_status_s sl_bt_evt_advertiser_periodic_advertising_status_t;

/** @} */ // end addtogroup sl_bt_evt_advertiser_periodic_advertising_status
/** @endcond */ // end restricted event

/***************************************************************************//**
 *
 * Create an advertising set. The handle of the created advertising set is
 * returned in response.
 *
 * @param[out] handle Advertising set handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_create_set(uint8_t *handle);

/***************************************************************************//**
 *
 * Set the advertising timing parameters of the given advertising set. This
 * setting will take effect next time that advertising is enabled.
 *
 * @param[in] handle Advertising set handle
 * @param[in] interval_min @parblock
 *   Minimum advertising interval. Value in units of 0.625 ms
 *     - Range: 0x20 to 0xFFFF
 *     - Time range: 20 ms to 40.96 s
 *
 *   Default value: 100 ms
 *   @endparblock
 * @param[in] interval_max @parblock
 *   Maximum advertising interval. Value in units of 0.625 ms
 *     - Range: 0x20 to 0xFFFF
 *     - Time range: 20 ms to 40.96 s
 *     - Note: interval_max should be bigger than interval_min
 *
 *   Default value: 200 ms
 *   @endparblock
 * @param[in] duration @parblock
 *   Advertising duration for this advertising set. Value 0 indicates no
 *   advertising duration limit and advertising continues until it is disabled.
 *   A non-zero value sets the duration in units of 10 ms. The duration begins
 *   at the start of the first advertising event of this advertising set.
 *     - Range: 0x0001 to 0xFFFF
 *     - Time range: 10 ms to 655.35 s
 *
 *   Default value: 0
 *   @endparblock
 * @param[in] maxevents @parblock
 *   If non-zero, indicates the maximum number of advertising events to send
 *   before the advertiser is stopped. Value 0 indicates no maximum number
 *   limit.
 *
 *   Default value: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_timing(uint8_t handle,
                                        uint32_t interval_min,
                                        uint32_t interval_max,
                                        uint16_t duration,
                                        uint8_t maxevents);

/***************************************************************************//**
 *
 * Set advertising PHYs of the given advertising set. This setting will take
 * effect next time that advertising is enabled. The invalid parameter error is
 * returned if a PHY value is invalid or the device does not support a given
 * PHY.
 *
 * @param[in] handle Advertising set handle
 * @param[in] primary_phy @parblock
 *   Enum @ref sl_bt_gap_phy_t. The PHY on which the advertising packets are
 *   transmitted on the primary advertising channel. If legacy advertising PDUs
 *   are used, 1M PHY must be used. Values:
 *     - <b>sl_bt_gap_phy_1m (0x1):</b> 1M PHY
 *     - <b>sl_bt_gap_phy_coded (0x4):</b> Coded PHY, 125k (S=8) or 500k (S=2)
 *
 *   Default value: @ref sl_bt_gap_phy_1m
 *   @endparblock
 * @param[in] secondary_phy @parblock
 *   Enum @ref sl_bt_gap_phy_t. The PHY on which the advertising packets are
 *   transmitted on the secondary advertising channel. Values:
 *     - <b>sl_bt_gap_phy_1m (0x1):</b> 1M PHY
 *     - <b>sl_bt_gap_phy_2m (0x2):</b> 2M PHY
 *     - <b>sl_bt_gap_phy_coded (0x4):</b> Coded PHY, 125k (S=8) or 500k (S=2)
 *
 *   Default value: @ref sl_bt_gap_phy_1m
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_phy(uint8_t handle,
                                     uint8_t primary_phy,
                                     uint8_t secondary_phy);

/***************************************************************************//**
 *
 * Set the primary advertising channel map of the given advertising set. This
 * setting will take effect next time that advertising is enabled.
 *
 * @param[in] handle Advertising set handle
 * @param[in] channel_map @parblock
 *   Advertising channel map which determines which of the three channels will
 *   be used for advertising. This value is given as a bitmask. Values:
 *     - <b>1:</b> Advertise on CH37
 *     - <b>2:</b> Advertise on CH38
 *     - <b>3:</b> Advertise on CH37 and CH38
 *     - <b>4:</b> Advertise on CH39
 *     - <b>5:</b> Advertise on CH37 and CH39
 *     - <b>6:</b> Advertise on CH38 and CH39
 *     - <b>7:</b> Advertise on all channels
 *
 *   Recommended value: 7
 *
 *   Default value: 7
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_channel_map(uint8_t handle,
                                             uint8_t channel_map);

/***************************************************************************//**
 *
 * Limit the maximum advertising TX power on the given advertising set. If the
 * value goes over the global value that was set using @ref
 * sl_bt_system_set_max_tx_power command, the global value will be the maximum
 * limit. The maximum TX power of legacy advertising is further constrained to
 * be less than +10 dBm. Extended advertising TX power can be +10 dBm and over
 * if Adaptive Frequency Hopping is enabled.
 *
 * This setting will take effect next time advertising is enabled.
 *
 * By default, maximum advertising TX power is limited by the global value.
 *
 * @param[in] handle Advertising set handle
 * @param[in] power TX power in 0.1 dBm steps. For example, the value of 10 is 1
 *   dBm and 55 is 5.5 dBm.
 * @param[out] set_power The selected maximum advertising TX power
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_tx_power(uint8_t handle,
                                          int16_t power,
                                          int16_t *set_power);

/***************************************************************************//**
 *
 * Enable or disable the scan request notification of a given advertising set.
 * This setting will take effect next time that advertising is enabled.
 *
 * @param[in] handle Advertising set handle
 * @param[in] report_scan_req @parblock
 *   If non-zero, enables scan request notification and scan requests will be
 *   reported as events.
 *
 *   Default value: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_advertiser_scan_request - Triggered when a scan request is
 *     received during advertising if the scan request notification is enabled
 *     by this command.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_report_scan_request(uint8_t handle,
                                                     uint8_t report_scan_req);

/***************************************************************************//**
 *
 * Set the advertiser on an advertising set to use a random address. This
 * overrides the default advertiser address which is either the public device
 * address programmed at production or the address written into persistent
 * storage using @ref sl_bt_system_set_identity_address command. This setting is
 * stored in RAM only and does not change the identity address in persistent
 * storage. In privacy mode, the stack does not change an advertiser address set
 * by this command. In order for the stack to manage the address update
 * periodically in privacy mode, the address setting should be removed with the
 * @ref sl_bt_advertiser_clear_random_address command.
 *
 * When setting a resolvable random address, the address parameter is ignored.
 * The stack generates one and set it as the advertiser address. The generated
 * address is returned in the response. To enhance the privacy, the application
 * should schedule periodic address updates by calling this command
 * periodically. It is recommended to use different schedules for different
 * advertising sets.
 *
 * To use the default advertiser address, remove this setting using @ref
 * sl_bt_advertiser_clear_random_address command.
 *
 * Wrong state error is returned if advertising has been enabled on the
 * advertising set. Invalid parameter error is returned if the advertising set
 * handle is invalid or the address does not conforms to the Bluetooth
 * specification.
 *
 * @param[in] handle Advertising set handle
 * @param[in] addr_type Address type:
 *     - <b>1:</b> Static device address
 *     - <b>2:</b> Resolvable private random address
 *     - <b>3:</b> Non-resolvable private random address. This type can only be
 *       used for non-connectable advertising.
 * @param[in] address The random address to set. Ignore this field when setting
 *   a resolvable random address.
 * @param[out] address_out The resolvable random address set for the advetiser.
 *   Ignore this field when setting other types of random address.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_random_address(uint8_t handle,
                                                uint8_t addr_type,
                                                bd_addr address,
                                                bd_addr *address_out);

/***************************************************************************//**
 *
 * Clear the random address previously set for the advertiser address on an
 * advertising set. A random address can be set using @ref
 * sl_bt_advertiser_set_random_address command. The default advertiser address
 * will be used after this operation.
 *
 * The error SL_STATUS_INVALID_STATE is returned if advertising has been enabled
 * on the advertising set. An invalid parameter error is returned if the
 * advertising set handle is invalid.
 *
 * @param[in] handle Advertising set handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_clear_random_address(uint8_t handle);

/***************************************************************************//**
 *
 * Enable advertising configuration flags on the given advertising set. The
 * configuration change will take effect next time that advertising is enabled.
 *
 * These configuration flags can be disabled using @ref
 * sl_bt_advertiser_clear_configuration.
 *
 * @param[in] handle Advertising set handle
 * @param[in] configurations @parblock
 *   Advertising configuration flags to enable. This value can be a bitmask of
 *   multiple flags. Flags:
 *     - <b>1 (Bit 0):</b> Use legacy advertising PDUs.
 *     - <b>2 (Bit 1):</b> Omit advertiser's address from all PDUs (anonymous
 *       advertising). This flag is effective only in extended advertising.
 *     - <b>4 (Bit 2):</b> Use a non-resolvable private address. When this
 *       configuration is enabled, the advertising must use non-connectable
 *       mode. The stack generates a non-resolvable private address for the
 *       advertising set and the stack will update the address periodically when
 *       the privacy mode is enabled. This configuration is ignored if the
 *       advertiser address has been set with the @ref
 *       sl_bt_advertiser_set_random_address command.
 *     - <b>8 (Bit 3):</b> Include TX power in advertising packets. This flag is
 *       effective only in - extended advertising.
 *     - <b>16 (Bit 4):</b> Use the device identity address when the privacy
 *       mode is enabled in the stack. This configuration is ignored if the
 *       configuration of using non-resolvable private address is enabled or the
 *       advertising address has been set with the @ref
 *       sl_bt_advertiser_set_random_address command.
 *
 *   Default value: 1
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_configuration(uint8_t handle,
                                               uint32_t configurations);

/***************************************************************************//**
 *
 * Disable advertising configuration flags on the given advertising set. The
 * configuration change will take effect next time that advertising is enabled.
 *
 * These configuration flags can be enabled using @ref
 * sl_bt_advertiser_set_configuration.
 *
 * @param[in] handle Advertising set handle
 * @param[in] configurations Advertising configuration flags to disable. This
 *   value can be a bitmask of multiple flags. See @ref
 *   sl_bt_advertiser_set_configuration for possible flags.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_clear_configuration(uint8_t handle,
                                                 uint32_t configurations);

/***************************************************************************//**
 *
 * Set user-defined data in advertising packets, scan response packets, or
 * periodic advertising packets. Maximum 31 bytes of data can be set for legacy
 * advertising. Maximum 191 bytes of data can be set for connectable extended
 * advertising. Maximum 253 bytes of data can be set for periodic and
 * non-connectable extended advertising. For setting longer advertising data,
 * use command @ref sl_bt_advertiser_set_long_data.
 *
 * If advertising mode is currently enabled, the new advertising data will be
 * used immediately. Advertising mode can be enabled using command @ref
 * sl_bt_advertiser_start. Periodic advertising mode can be enabled using
 * command @ref sl_bt_advertiser_start_periodic_advertising.
 *
 * The invalid parameter error will be returned in the following situations:
 *   - Data length is more than 31 bytes but the advertiser can only advertise
 *     using legacy advertising PDUs.
 *   - Data is too long to fit into a single advertisement.
 *   - Set data of the advertising data packet when the advertiser is
 *     advertising in scannable mode using extended advertising PDUs.
 *   - Set data of the scan response data packet when the advertiser is
 *     advertising in connectable mode using extended advertising PDUs.
 *
 * Note that the user-defined data may be overwritten by the system when the
 * advertising is later enabled in a discoverable mode other than user_data.
 *
 * @param[in] handle Advertising set handle
 * @param[in] packet_type This value selects whether data is intended for
 *   advertising packets, scan response packets, or periodic advertising
 *   packets.
 *     - <b>0:</b> Advertising packets
 *     - <b>1:</b> Scan response packets
 *     - <b>8:</b> Periodic advertising packets
 * @param[in] adv_data_len Length of data in @p adv_data
 * @param[in] adv_data Data to be set
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_data(uint8_t handle,
                                      uint8_t packet_type,
                                      size_t adv_data_len,
                                      const uint8_t* adv_data);

/***************************************************************************//**
 *
 * Set advertising data for a specified packet type and advertising set. Data
 * currently in the system data buffer will be extracted as the advertising
 * data. The buffer will be emptied after this command regardless of the
 * completion status.
 *
 * Prior to calling this command, add data to the buffer with one or multiple
 * calls to @ref sl_bt_system_data_buffer_write.
 *
 * Maximum 31 bytes of data can be set for legacy advertising. Maximum 191 bytes
 * of data can be set for connectable extended advertising. Maximum 1650 bytes
 * of data can be set for periodic and non-connectable extended advertising, but
 * advertising parameters may limit the amount of data that can be sent in a
 * single advertisement.
 *
 * See @ref sl_bt_advertiser_set_data for more details on advertising data.
 *
 * @param[in] handle Advertising set handle
 * @param[in] packet_type This value selects whether data is intended for
 *   advertising packets, scan response packets, or periodic advertising
 *   packets. Values:
 *     - <b>0:</b> Advertising packets
 *     - <b>1:</b> Scan response packets
 *     - <b>8:</b> Periodic advertising packets
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_long_data(uint8_t handle, uint8_t packet_type);

/***************************************************************************//**
 *
 * Start advertising of a given advertising set with specified discoverable and
 * connectable modes.
 *
 * The number of concurrent advertising is limited by MAX_ADVERTISERS
 * configuration.
 *
 * The number of concurrent connectable advertising is also limited by
 * MAX_CONNECTIONS configuration. For example, only one connectable advertising
 * can be enabled if the device has (MAX_CONNECTIONS - 1) connections when this
 * command is called. The limitation does not apply to non-connectable
 * advertising.
 *
 * The default advertising configuration in the stack is set to using legacy
 * advertising PDUs on 1M PHY. The stack will automatically select extended
 * advertising PDUs if either of the following has occurred with the default
 * configuration:
 *   1. The connectable mode is set to advertiser_connectable_non_scannable.
 *   2. The primary advertising PHY is set to Coded PHY by @ref
 *      sl_bt_advertiser_set_phy.
 *   3. The user advertising data length is more than 31 bytes.
 *   4. Periodic advertising is enabled.
 *
 * If the currently set parameters can't be used, an error is returned.
 * Specifically, this command fails with the connection limit exceeded error if
 * it causes the number of connections exceeding the configured MAX_CONNECTIONS
 * value. It fails with the invalid parameter error if one of the following use
 * cases occurs:
 *   1. Non-resolvable random address is used but the connectable mode is
 *      advertiser_connectable_scannable or
 *      advertiser_connectable_non_scannable.
 *   2. advertiser_connectable_non_scannable is the connectable mode but using
 *      legacy advertising PDUs has been explicitly enabled with command @ref
 *      sl_bt_advertiser_set_configuration.
 *   3. Coded PHY is the primary advertising PHY but using legacy advertising
 *      PDUs has been explicitly enabled with command @ref
 *      sl_bt_advertiser_set_configuration.
 *   4. advertiser_connectable_scannable is the connectable mode but using
 *      extended advertising PDUs has been explicitly enabled or the primary
 *      advertising PHY is set to Coded PHY.
 *
 * If advertising is enabled in user_data mode, use @ref
 * sl_bt_advertiser_set_data to set advertising and scan response data before
 * issuing this command. When advertising is enabled in modes other than
 * user_data, advertising and scan response data is generated by the stack using
 * the following procedure:
 *   1. Add a flags field to advertising data.
 *   2. Add a TX power level field to advertising data if the TX power service
 *      exists in the local GATT database.
 *   3. Add a peripheral connection interval range field to advertising data if
 *      the GAP peripheral preferred connection parameters characteristic exists
 *      in the local GATT database.
 *   4. Add a list of 16-bit service UUIDs to advertising data if there are one
 *      or more 16-bit service UUIDs to advertise. The list is complete if all
 *      advertised 16-bit UUIDs are in advertising data. Otherwise, the list is
 *      incomplete.
 *   5. Add a list of 128-bit service UUIDs to advertising data if there are one
 *      or more 128-bit service UUIDs to advertise and there is still free space
 *      for this field. The list is complete if all advertised 128-bit UUIDs are
 *      in advertising data. Otherwise, the list is incomplete. Note that an
 *      advertising data packet can contain at most one 128-bit service UUID.
 *   6. Try to add the full local name to advertising data if the device is not
 *      in privacy mode. If the full local name does not fit into the remaining
 *      free space, the advertised name is a shortened version by cutting off
 *      the end if the free space has at least 6 bytes. Otherwise, the local
 *      name is added to scan response data.
 *
 * Event @ref sl_bt_evt_connection_opened will be received when a remote device
 * opens a connection to the advertiser on this advertising set and also
 * advertising on the given set stops.
 *
 * Event @ref sl_bt_evt_advertiser_timeout will be received when the number of
 * advertising events set by @ref sl_bt_advertiser_set_timing command is done
 * and advertising with the current set has stopped.
 *
 * @param[in] handle Advertising set handle
 * @param[in] discover Enum @ref sl_bt_advertiser_discoverable_mode_t.
 *   Discoverable mode. Values:
 *     - <b>sl_bt_advertiser_non_discoverable (0x0):</b> Not discoverable
 *     - <b>sl_bt_advertiser_limited_discoverable (0x1):</b> Discoverable using
 *       both limited and general discovery procedures
 *     - <b>sl_bt_advertiser_general_discoverable (0x2):</b> Discoverable using
 *       general discovery procedure
 *     - <b>sl_bt_advertiser_broadcast (0x3):</b> Device is not discoverable in
 *       either limited or generic discovery procedure but may be discovered
 *       using the Observation procedure.
 *     - <b>sl_bt_advertiser_user_data (0x4):</b> Send advertising and/or scan
 *       response data defined by the user. The limited/general discoverable
 *       flags are defined by the user.
 * @param[in] connect Enum @ref sl_bt_advertiser_connectable_mode_t. Connectable
 *   mode. Values:
 *     - <b>sl_bt_advertiser_non_connectable (0x0):</b> Non-connectable
 *       non-scannable.
 *     - <b>sl_bt_advertiser_directed_connectable (0x1):</b> Directed
 *       connectable <b>(RESERVED, DO NOT USE)</b>
 *     - <b>sl_bt_advertiser_connectable_scannable (0x2):</b> Undirected
 *       connectable scannable. This mode can only be used in legacy advertising
 *       PDUs.
 *     - <b>sl_bt_advertiser_scannable_non_connectable (0x3):</b> Undirected
 *       scannable (Non-connectable but responds to scan requests)
 *     - <b>sl_bt_advertiser_connectable_non_scannable (0x4):</b> Undirected
 *       connectable non-scannable. This mode can only be used in extended
 *       advertising PDUs.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_advertiser_timeout - Triggered when the number of
 *     advertising events set by @ref sl_bt_advertiser_set_timing command is
 *     done and advertising has stopped on the given advertising set.
 *   - @ref sl_bt_evt_connection_opened - Triggered when a remote device opens a
 *     connection to the advertiser on the specified advertising set and also
 *     advertising with the current set stops.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_start(uint8_t handle,
                                   uint8_t discover,
                                   uint8_t connect);

/***************************************************************************//**
 *
 * Stop the advertising of the given advertising set. Counterpart with @ref
 * sl_bt_advertiser_start.
 *
 * This command does not affect the enable state of the periodic advertising
 * set, i.e., periodic advertising is not stopped.
 *
 * @param[in] handle Advertising set handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_stop(uint8_t handle);

/***************************************************************************//**
 *
 * Start periodic advertising on the given advertising set. The stack enables
 * the advertising set automatically if the set was not enabled and the set can
 * advertise using extended advertising PDUs beside the syncInfo, which is
 * needed for the periodic advertising.
 *
 * The invalid parameter error is returned if the application has configured
 * legacy advertising PDUs or anonymous advertising, or the advertising set is
 * enabled using legacy advertising PDUs.
 *
 * To stop periodic advertising, use @ref
 * sl_bt_advertiser_stop_periodic_advertising command with the handle received
 * in response from this command.
 *
 * @param[in] handle Advertising set handle
 * @param[in] interval_min @parblock
 *   Minimum periodic advertising interval. Value in units of 1.25 ms
 *     - Range: 0x06 to 0xFFFF
 *     - Time range: 7.5 ms to 81.92 s
 *
 *   Default value: 100 ms
 *   @endparblock
 * @param[in] interval_max @parblock
 *   Maximum periodic advertising interval. Value in units of 1.25 ms
 *     - Range: 0x06 to 0xFFFF
 *     - Time range: 7.5 ms to 81.92 s
 *     - Note: interval_max should be bigger than interval_min
 *
 *   Default value: 200 ms
 *   @endparblock
 * @param[in] flags Periodic advertising configurations. Bitmask of the
 *   following:
 *     - <b>Bit 0:</b> Include TX power in advertising PDU
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_start_periodic_advertising(uint8_t handle,
                                                        uint16_t interval_min,
                                                        uint16_t interval_max,
                                                        uint32_t flags);

/***************************************************************************//**
 *
 * Stop the periodic advertising on the given advertising set. Counterpart with
 * @ref sl_bt_advertiser_start_periodic_advertising.
 *
 * This command does not affect the enable state of the advertising set, i.e.,
 * legacy or extended advertising is not stopped.
 *
 * @param[in] handle Advertising set handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_stop_periodic_advertising(uint8_t handle);

/***************************************************************************//**
 *
 * Delete an advertising set.
 *
 * @param[in] handle Advertising set handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_delete_set(uint8_t handle);

/** @} */ // end addtogroup sl_bt_advertiser

/**
 * @addtogroup sl_bt_scanner Scanner
 * @{
 *
 * @brief Scanner
 *
 * The commands and events in this class are related to scanning functionalities
 * in GAP central and observer roles.
 */

/* Command and Response IDs */
#define sl_bt_cmd_scanner_set_timing_id                              0x01050020
#define sl_bt_cmd_scanner_set_mode_id                                0x02050020
#define sl_bt_cmd_scanner_start_id                                   0x03050020
#define sl_bt_cmd_scanner_stop_id                                    0x05050020
#define sl_bt_rsp_scanner_set_timing_id                              0x01050020
#define sl_bt_rsp_scanner_set_mode_id                                0x02050020
#define sl_bt_rsp_scanner_start_id                                   0x03050020
#define sl_bt_rsp_scanner_stop_id                                    0x05050020

/**
 * @brief 
                These values indicate which Bluetooth discovery mode to use when scanning for advertising devices.
            
 */
typedef enum
{
  sl_bt_scanner_discover_limited     = 0x0, /**< (0x0) Discover only limited
                                                 discoverable devices. */
  sl_bt_scanner_discover_generic     = 0x1, /**< (0x1) Discover limited and
                                                 generic discoverable devices. */
  sl_bt_scanner_discover_observation = 0x2  /**< (0x2) Discover all devices. */
} sl_bt_scanner_discover_mode_t;

/**
 * @addtogroup sl_bt_evt_scanner_scan_report sl_bt_evt_scanner_scan_report
 * @{
 * @brief Reports an advertising or scan response packet that is received by the
 * device's radio while in scanning mode
 */

/** @brief Identifier of the scan_report event */
#define sl_bt_evt_scanner_scan_report_id                             0x010500a0

/***************************************************************************//**
 * @brief Data structure of the scan_report event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_scanner_scan_report_s
{
  uint8_t    packet_type;       /**< <b>Bits 0..2</b> : advertising packet type
                                       - <b>000</b> : Connectable scannable
                                         undirected advertising
                                       - <b>001</b> : Connectable undirected
                                         advertising
                                       - <b>010</b> : Scannable undirected
                                         advertising
                                       - <b>011</b> : Non-connectable
                                         non-scannable undirected advertising
                                       - <b>100</b> : Scan Response. Note that
                                         this is received only if the device is
                                         in active scan mode.

                                     <b>Bits 3..4</b> : Reserved for future

                                     <b>Bits 5..6</b> : data completeness
                                       - <b>00:</b> Complete
                                       - <b>01:</b> Incomplete, more data to
                                         come in new events
                                       - <b>10:</b> Incomplete, data truncated,
                                         no more to come

                                     <b>Bit 7</b> : legacy or extended
                                     advertising
                                       - <b>0:</b> Legacy advertising PDUs used
                                       - <b>1:</b> Extended advertising PDUs
                                         used */
  bd_addr    address;           /**< Bluetooth address of the remote device */
  uint8_t    address_type;      /**< Advertiser address type. Values:
                                       - <b>0:</b> Public address
                                       - <b>1:</b> Random address
                                       - <b>255:</b> No address provided
                                         (anonymous advertising) */
  uint8_t    bonding;           /**< Bonding handle if the remote advertising
                                     device has previously bonded with the local
                                     device. Values:
                                       - <b>SL_BT_INVALID_BONDING_HANDLE
                                         (0xff):</b> No bonding
                                       - <b>Other:</b> Bonding handle */
  uint8_t    primary_phy;       /**< Enum @ref sl_bt_gap_phy_t. The PHY on which
                                     advertising packets are transmitted on the
                                     primary advertising channel. Ignore this
                                     field if the report is for a legacy
                                     advertising PDU. Values:
                                       - <b>sl_bt_gap_phy_1m (0x1):</b> 1M PHY
                                       - <b>sl_bt_gap_phy_coded (0x4):</b> Coded
                                         PHY, 125k (S=8) or 500k (S=2) */
  uint8_t    secondary_phy;     /**< Enum @ref sl_bt_gap_phy_t. The PHY on which
                                     advertising packets are transmitted on the
                                     secondary advertising channel. Ignore this
                                     field if the report is for a legacy
                                     advertising PDU. Values:
                                       - <b>sl_bt_gap_phy_1m (0x1):</b> 1M PHY
                                       - <b>sl_bt_gap_phy_2m (0x2):</b> 2M PHY
                                       - <b>sl_bt_gap_phy_coded (0x4):</b> Coded
                                         PHY, 125k (S=8) or 500k (S=2) */
  uint8_t    adv_sid;           /**< Advertising set identifier */
  int8_t     tx_power;          /**< TX power value in the received packet
                                     header. Units: dBm
                                       - Valid value range: -127 to 126
                                       - Value 127: information unavailable */
  int8_t     rssi;              /**< Signal strength indicator (RSSI) in the
                                     last received packet. Units: dBm
                                       - Range: -127 to +20 */
  uint8_t    channel;           /**< The channel number on which the last packet
                                     was received */
  uint16_t   periodic_interval; /**< The periodic advertising interval. Value 0
                                     indicates no periodic advertising.
                                     Otherwise,
                                       - Range: 0x06 to 0xFFFF
                                       - Unit: 1.25 ms
                                       - Time range: 7.5 ms to 81.92 s */
  uint8array data;              /**< Advertising or scan response data */
});

typedef struct sl_bt_evt_scanner_scan_report_s sl_bt_evt_scanner_scan_report_t;

/** @} */ // end addtogroup sl_bt_evt_scanner_scan_report

/***************************************************************************//**
 *
 * Set the scanning timing parameters on the specified PHYs. If the device is
 * currently scanning for advertising devices on PHYs, new parameters will take
 * effect when scanning is restarted.
 *
 * @param[in] phys PHYs for which the parameters are set.
 *     - <b>1:</b> 1M PHY
 *     - <b>4:</b> Coded PHY
 *     - <b>5:</b> 1M PHY and Coded PHY
 * @param[in] scan_interval @parblock
 *   Scan interval is defined as the time interval when the device starts its
 *   last scan until it begins the subsequent scan. In other words, how often to
 *   scan
 *     - Time = Value x 0.625 ms
 *     - Range: 0x0004 to 0xFFFF
 *     - Time Range: 2.5 ms to 40.96 s
 *
 *   Default value: 10 ms
 *
 *   A variable delay occurs when switching channels at the end of each scanning
 *   interval, which is included in the scanning interval time. During the
 *   switch time, advertising packets are not received by the device. The switch
 *   time variation is use case-dependent. For example, if scanning while
 *   keeping active connections, the channel switch time might be longer than
 *   when scanning without any active connections. Increasing the scanning
 *   interval reduces the amount of time in which the device can't receive
 *   advertising packets because it switches channels less often.
 *
 *   After every scan interval, the scanner changes the frequency at which it
 *   operates. It cycles through all three advertising channels in a round robin
 *   fashion. According to the specification, all three channels must be used by
 *   a scanner.
 *   @endparblock
 * @param[in] scan_window @parblock
 *   Scan window defines the duration of the scan which must be less than or
 *   equal to the @p scan_interval
 *     - Time = Value x 0.625 ms
 *     - Range: 0x0004 to 0xFFFF
 *     - Time Range: 2.5 ms to 40.96 s
 *
 *   Default value: 10 ms Note that the packet reception is aborted if it's
 *   started just before the scan window ends.
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_scanner_set_timing(uint8_t phys,
                                     uint16_t scan_interval,
                                     uint16_t scan_window);

/***************************************************************************//**
 *
 * Set the scan mode on the specified PHYs. If the device is currently scanning
 * for advertising devices on PHYs, new parameters will take effect when
 * scanning is restarted.
 *
 * @param[in] phys PHYs for which the parameters are set.
 *     - <b>1:</b> 1M PHY
 *     - <b>4:</b> Coded PHY
 *     - <b>5:</b> 1M PHY and Coded PHY
 * @param[in] scan_mode @parblock
 *   Scan mode. Values:
 *     - <b>0:</b> Passive scanning
 *     - <b>1:</b> Active scanning
 *     - In passive scanning mode, the device only listens to advertising
 *       packets and does not transmit packets.
 *     - In active scanning mode, the device sends out a scan request packet
 *       upon receiving an advertising packet from a remote device. Then, it
 *       listens to the scan response packet from the remote device.
 *
 *   Default value: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_scanner_set_mode(uint8_t phys, uint8_t scan_mode);

/***************************************************************************//**
 *
 * Start the GAP discovery procedure to scan for advertising devices on the
 * specified scanning PHYs. To cancel an ongoing discovery procedure, use the
 * @ref sl_bt_scanner_stop command.
 *
 * The invalid parameter error will be returned if the value of scanning PHYs is
 * invalid or the device does not support a PHY.
 *
 * @param[in] scanning_phy @parblock
 *   The scanning PHYs.
 *
 *   In simultaneous scanning, the stack alternates the scanning on two PHYs by
 *   switching the PHY at every scan interval. When a timing parameter is set
 *   differently on 1M and Coded PHY, the stack chooses the most relaxed value
 *   for both PHYs during simultaneous scanning, i.e., the largest scan interval
 *   or the smallest scan window. If one PHY is set to passive scanning and the
 *   other to active scanning, passive scanning is chosen for simultaneous
 *   scanning.
 *
 *     - <b>1:</b> 1M PHY
 *     - <b>4:</b> Coded PHY
 *     - <b>5:</b> 1M PHY and Coded PHY. The stack performs simultaneous
 *       scanning on both PHYs.
 *   @endparblock
 * @param[in] discover_mode Enum @ref sl_bt_scanner_discover_mode_t. Bluetooth
 *   discovery Mode. Values:
 *     - <b>sl_bt_scanner_discover_limited (0x0):</b> Discover only limited
 *       discoverable devices.
 *     - <b>sl_bt_scanner_discover_generic (0x1):</b> Discover limited and
 *       generic discoverable devices.
 *     - <b>sl_bt_scanner_discover_observation (0x2):</b> Discover all devices.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_scanner_scan_report - This event is triggered each time an
 *     advertising packet is received. Packets are not filtered in any way, so
 *     multiple events will be received for every advertising device in range.
 *
 ******************************************************************************/
sl_status_t sl_bt_scanner_start(uint8_t scanning_phy, uint8_t discover_mode);

/***************************************************************************//**
 *
 * Stop scanning for advertising devices. For more information about the
 * discovery, see the @ref sl_bt_scanner_start command.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_scanner_stop();

/** @} */ // end addtogroup sl_bt_scanner

/**
 * @addtogroup sl_bt_sync Synchronization
 * @{
 *
 * @brief Synchronization
 *
 * Provides periodic advertising synchronization feature.
 */

/* Command and Response IDs */
#define sl_bt_cmd_sync_set_parameters_id                             0x02420020
#define sl_bt_cmd_sync_open_id                                       0x00420020
#define sl_bt_cmd_sync_close_id                                      0x01420020
#define sl_bt_rsp_sync_set_parameters_id                             0x02420020
#define sl_bt_rsp_sync_open_id                                       0x00420020
#define sl_bt_rsp_sync_close_id                                      0x01420020

/**
 * @brief 
                These values indicate the advertiser clock accuracy in a periodic advertising synchronization.
            
 */
typedef enum
{
  sl_bt_sync_clock_accuracy_500 = 0x1f4, /**< (0x1f4) Clock accuracy 500 ppm */
  sl_bt_sync_clock_accuracy_250 = 0xfa,  /**< (0xfa) Clock accuracy 250 ppm */
  sl_bt_sync_clock_accuracy_150 = 0x96,  /**< (0x96) Clock accuracy 150 ppm */
  sl_bt_sync_clock_accuracy_100 = 0x64,  /**< (0x64) Clock accuracy 100 ppm */
  sl_bt_sync_clock_accuracy_75  = 0x4b,  /**< (0x4b) Clock accuracy 75 ppm */
  sl_bt_sync_clock_accuracy_50  = 0x32,  /**< (0x32) Clock accuracy 50 ppm */
  sl_bt_sync_clock_accuracy_30  = 0x1e,  /**< (0x1e) Clock accuracy 30 ppm */
  sl_bt_sync_clock_accuracy_20  = 0x14   /**< (0x14) Clock accuracy 20 ppm */
} sl_bt_sync_advertiser_clock_accuracy_t;

/**
 * @addtogroup sl_bt_evt_sync_opened sl_bt_evt_sync_opened
 * @{
 * @brief Indicates that a periodic advertising synchronization has been opened.
 */

/** @brief Identifier of the opened event */
#define sl_bt_evt_sync_opened_id                                     0x004200a0

/***************************************************************************//**
 * @brief Data structure of the opened event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sync_opened_s
{
  uint16_t sync;           /**< Periodic advertising synchronization handle */
  uint8_t  adv_sid;        /**< Advertising set identifier */
  bd_addr  address;        /**< Address of the advertiser */
  uint8_t  address_type;   /**< Advertiser address type. Values:
                                  - <b>0:</b> Public address
                                  - <b>1:</b> Random address */
  uint8_t  adv_phy;        /**< Enum @ref sl_bt_gap_phy_t. The advertiser PHY.
                                Values:
                                  - <b>sl_bt_gap_phy_1m (0x1):</b> 1M PHY
                                  - <b>sl_bt_gap_phy_2m (0x2):</b> 2M PHY
                                  - <b>sl_bt_gap_phy_coded (0x4):</b> Coded PHY,
                                    125k (S=8) or 500k (S=2) */
  uint16_t adv_interval;   /**< The periodic advertising interval. Value in
                                units of 1.25 ms
                                  - Range: 0x06 to 0xFFFF
                                  - Time range: 7.5 ms to 81.92 s */
  uint16_t clock_accuracy; /**< Enum @ref
                                sl_bt_sync_advertiser_clock_accuracy_t. The
                                advertiser clock accuracy. */
  uint8_t  bonding;        /**< Bonding handle. Values:
                                  - <b>SL_BT_INVALID_BONDING_HANDLE (0xff):</b>
                                    No bonding
                                  - <b>Other:</b> Bonding handle */
});

typedef struct sl_bt_evt_sync_opened_s sl_bt_evt_sync_opened_t;

/** @} */ // end addtogroup sl_bt_evt_sync_opened

/**
 * @addtogroup sl_bt_evt_sync_data sl_bt_evt_sync_data
 * @{
 * @brief Reports a received periodic advertisement packet
 */

/** @brief Identifier of the data event */
#define sl_bt_evt_sync_data_id                                       0x024200a0

/***************************************************************************//**
 * @brief Data structure of the data event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sync_data_s
{
  uint16_t   sync;        /**< Periodic advertising synchronization handle */
  int8_t     tx_power;    /**< TX power value in the received packet header.
                               Units: dBm
                                 - Valid value range: -127 to 126
                                 - Value 127: information unavailable */
  int8_t     rssi;        /**< Signal strength indicator (RSSI) in the latest
                               received packet. Units: dBm
                                 - Range: -127 to +20 */
  uint8_t    data_status; /**< Data completeness:
                                 - <b>0:</b> Complete
                                 - <b>1:</b> Incomplete, more data to come in
                                   new events
                                 - <b>2:</b> Incomplete, data truncated, no more
                                   to come */
  uint8array data;        /**< Periodic advertising data */
});

typedef struct sl_bt_evt_sync_data_s sl_bt_evt_sync_data_t;

/** @} */ // end addtogroup sl_bt_evt_sync_data

/**
 * @addtogroup sl_bt_evt_sync_closed sl_bt_evt_sync_closed
 * @{
 * @brief Indicates that periodic advertising synchronization was lost or a
 * synchronization establishment procedure was canceled
 *
 * The synchronization establishment procedure can be canceled explicitly by the
 * application by issuing command @ref sl_bt_sync_close, or internally due to
 * synchronization failing. Synchronization can fail for example due to
 * incompatible sync CTE type.
 */

/** @brief Identifier of the closed event */
#define sl_bt_evt_sync_closed_id                                     0x014200a0

/***************************************************************************//**
 * @brief Data structure of the closed event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sync_closed_s
{
  uint16_t reason; /**< Result code
                          - <b>0:</b> success
                          - <b>Non-zero:</b> an error has occurred */
  uint16_t sync;   /**< Periodic advertising synchronization handle */
});

typedef struct sl_bt_evt_sync_closed_s sl_bt_evt_sync_closed_t;

/** @} */ // end addtogroup sl_bt_evt_sync_closed

/***************************************************************************//**
 *
 * Configure periodic advertiser synchronization parameters. The specified
 * parameters take effect immediately for all advertisers that have not already
 * established synchronization.
 *
 * The application should determine skip and timeout values based on the
 * periodic advertising interval provided by the advertiser. It is recommended
 * to use a long enough timeout that allows multiple receives. If @p skip and @p
 * timeout are used, select appropriate values so that they allow a few
 * receiving attempts. Periodic advertising intervals are reported in event @ref
 * sl_bt_evt_scanner_scan_report.
 *
 * @param[in] skip The maximum number of periodic advertising packets that can
 *   be skipped after a successful receive.
 *     - Range: 0x0000 to 0x01F3
 *     - Default value: 0
 * @param[in] timeout The maximum permitted time between successful receives. If
 *   this time is exceeded, synchronization is lost. Unit: 10 ms.
 *     - Range: 0x0A to 0x4000
 *     - Unit: 10 ms
 *     - Time range: 100 ms to 163.84 s
 *     - Default value: 1000 ms
 * @param[in] flags No flags defined currently
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sync_set_parameters(uint16_t skip,
                                      uint16_t timeout,
                                      uint32_t flags);

/***************************************************************************//**
 *
 * Start establishing synchronization with the specified periodic advertiser in
 * parallel with other advertisers given in previous invocations of this
 * command. The stack will internally enable scanning when needed so that
 * synchronizations can occur. The scanning responses from the internal scanning
 * are not passed to the application unless the application has also enabled
 * scanning.
 *
 * Advertisers that have not already synced before the invocation of this
 * command will be synced using the @p skip and @p timeout values configured in
 * the most recent invocation of command @ref sl_bt_evt_scanner_scan_report.
 *
 * @param[in] address Address of the advertiser
 * @param[in] address_type Advertiser address type. Values:
 *     - <b>0:</b> Public address
 *     - <b>1:</b> Random address
 * @param[in] adv_sid Advertising set identifier
 * @param[out] sync A handle that will be assigned to the periodic advertising
 *   synchronization after the synchronization is established. This handle is
 *   valid only if the result code of this response is SL_STATUS_OK.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_sync_opened - Triggered after the synchronization is
 *     established.
 *   - @ref sl_bt_evt_sync_data - Indicates that a periodic advertisement packet
 *     is received.
 *   - @ref sl_bt_evt_sync_closed - Triggered after periodic advertising
 *     synchronization was lost or explicitly closed, or a synchronization
 *     establishment procedure was canceled.
 *
 ******************************************************************************/
sl_status_t sl_bt_sync_open(bd_addr address,
                            uint8_t address_type,
                            uint8_t adv_sid,
                            uint16_t *sync);

/***************************************************************************//**
 *
 * Closes a periodic advertising synchronization or cancels an ongoing attempt
 * of establishing a synchronization.
 *
 * @param[in] sync Periodic advertising synchronization handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_sync_closed - Triggered after a periodic advertising
 *     synchronization has been closed or canceled.
 *
 ******************************************************************************/
sl_status_t sl_bt_sync_close(uint16_t sync);

/** @} */ // end addtogroup sl_bt_sync

/**
 * @addtogroup sl_bt_connection Connection
 * @{
 *
 * @brief Connection
 *
 * The commands and events in this class are related to managing connection
 * establishment, parameter setting, and disconnection procedures.
 */

/* Command and Response IDs */
#define sl_bt_cmd_connection_set_default_parameters_id               0x00060020
#define sl_bt_cmd_connection_set_default_preferred_phy_id            0x01060020
#define sl_bt_cmd_connection_open_id                                 0x04060020
#define sl_bt_cmd_connection_set_parameters_id                       0x06060020
#define sl_bt_cmd_connection_set_preferred_phy_id                    0x08060020
#define sl_bt_cmd_connection_disable_slave_latency_id                0x03060020
#define sl_bt_cmd_connection_get_rssi_id                             0x02060020
#define sl_bt_cmd_connection_read_channel_map_id                     0x07060020
#define sl_bt_cmd_connection_set_power_reporting_id                  0x09060020
#define sl_bt_cmd_connection_set_remote_power_reporting_id           0x0a060020
#define sl_bt_cmd_connection_get_tx_power_id                         0x0b060020
#define sl_bt_cmd_connection_get_remote_tx_power_id                  0x0c060020
#define sl_bt_cmd_connection_close_id                                0x05060020
#define sl_bt_cmd_connection_read_remote_used_features_id            0x0d060020
#define sl_bt_rsp_connection_set_default_parameters_id               0x00060020
#define sl_bt_rsp_connection_set_default_preferred_phy_id            0x01060020
#define sl_bt_rsp_connection_open_id                                 0x04060020
#define sl_bt_rsp_connection_set_parameters_id                       0x06060020
#define sl_bt_rsp_connection_set_preferred_phy_id                    0x08060020
#define sl_bt_rsp_connection_disable_slave_latency_id                0x03060020
#define sl_bt_rsp_connection_get_rssi_id                             0x02060020
#define sl_bt_rsp_connection_read_channel_map_id                     0x07060020
#define sl_bt_rsp_connection_set_power_reporting_id                  0x09060020
#define sl_bt_rsp_connection_set_remote_power_reporting_id           0x0a060020
#define sl_bt_rsp_connection_get_tx_power_id                         0x0b060020
#define sl_bt_rsp_connection_get_remote_tx_power_id                  0x0c060020
#define sl_bt_rsp_connection_close_id                                0x05060020
#define sl_bt_rsp_connection_read_remote_used_features_id            0x0d060020

/**
 * @brief Indicate the Bluetooth Security Mode.
 */
typedef enum
{
  sl_bt_connection_mode1_level1 = 0x0, /**< (0x0) No security */
  sl_bt_connection_mode1_level2 = 0x1, /**< (0x1) Unauthenticated pairing with
                                            encryption */
  sl_bt_connection_mode1_level3 = 0x2, /**< (0x2) Authenticated pairing with
                                            encryption */
  sl_bt_connection_mode1_level4 = 0x3  /**< (0x3) Authenticated Secure
                                            Connections pairing with encryption
                                            using a 128-bit strength encryption
                                            key */
} sl_bt_connection_security_t;

/**
 * @brief 
                These values define transmit power reporting modes in LE power control.
            
 */
typedef enum
{
  sl_bt_connection_power_reporting_disable = 0x0, /**< (0x0) Disable transmit
                                                       power reporting */
  sl_bt_connection_power_reporting_enable  = 0x1  /**< (0x1) Enable transmit
                                                       power reporting */
} sl_bt_connection_power_reporting_mode_t;

/**
 * @brief 
                Transmit power level flags
            
 */
typedef enum
{
  sl_bt_connection_tx_power_at_minimum = 0x1, /**< (0x1) Transmit power level is
                                                   at minimum level. */
  sl_bt_connection_tx_power_at_maximum = 0x2  /**< (0x2) Transmit power level is
                                                   at maximum level. */
} sl_bt_connection_tx_power_flag_t;

/**
 * @addtogroup sl_bt_connection_tx_power_const Transmit Power Reporting Constants
 * @{
 *
 * Constants in transmit power reporting
 */

/** Remote device is not managing power levels. */
#define SL_BT_CONNECTION_TX_POWER_UNMANAGED          0x7e      

/** Transmit power level is not available. */
#define SL_BT_CONNECTION_TX_POWER_UNAVAILABLE        0x7f      

/** Change is not available or is out of range. */
#define SL_BT_CONNECTION_TX_POWER_CHANGE_UNAVAILABLE 0x7f      

/** @} */ // end Transmit Power Reporting Constants

/**
 * @addtogroup sl_bt_evt_connection_opened sl_bt_evt_connection_opened
 * @{
 * @brief Indicates that a new connection was opened
 *
 * This event does not indicate that the connection was established (i.e., that
 * a data packet was received within 6 connection interval). If the connection
 * does not get established, an @ref sl_bt_evt_connection_closed event may
 * immediately follow. This event also reports whether the connected devices are
 * already bonded and what the role of the Bluetooth device (central or
 * peripheral) is. An open connection can be closed with @ref
 * sl_bt_connection_close command by giving the connection handle obtained from
 * this event.
 */

/** @brief Identifier of the opened event */
#define sl_bt_evt_connection_opened_id                               0x000600a0

/***************************************************************************//**
 * @brief Data structure of the opened event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_connection_opened_s
{
  bd_addr address;      /**< Remote device address */
  uint8_t address_type; /**< Enum @ref sl_bt_gap_address_type_t. Remote device
                             address type. Values:
                               - <b>sl_bt_gap_public_address (0x0):</b> Public
                                 device address
                               - <b>sl_bt_gap_static_address (0x1):</b> Static
                                 device address
                               - <b>sl_bt_gap_random_resolvable_address
                                 (0x2):</b> Resolvable private random address
                               - <b>sl_bt_gap_random_nonresolvable_address
                                 (0x3):</b> Non-resolvable private random
                                 address */
  uint8_t master;       /**< Device role in connection. Values:
                               - <b>0:</b> Peripheral
                               - <b>1:</b> Central */
  uint8_t connection;   /**< Handle for new connection */
  uint8_t bonding;      /**< Bonding handle. Values:
                               - <b>SL_BT_INVALID_BONDING_HANDLE (0xff):</b> No
                                 bonding
                               - <b>Other:</b> Bonding handle */
  uint8_t advertiser;   /**< The local advertising set that this connection was
                             opened to. Values:
                               - <b>SL_BT_INVALID_ADVERTISING_SET_HANDLE
                                 (0xff):</b> Invalid value or not applicable.
                                 Ignore this field
                               - <b>Other:</b> The advertising set handle */
});

typedef struct sl_bt_evt_connection_opened_s sl_bt_evt_connection_opened_t;

/** @} */ // end addtogroup sl_bt_evt_connection_opened

/**
 * @addtogroup sl_bt_evt_connection_parameters sl_bt_evt_connection_parameters
 * @{
 * @brief Triggered whenever the connection parameters are changed and at any
 * time a connection is established
 */

/** @brief Identifier of the parameters event */
#define sl_bt_evt_connection_parameters_id                           0x020600a0

/***************************************************************************//**
 * @brief Data structure of the parameters event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_connection_parameters_s
{
  uint8_t  connection;    /**< Connection handle */
  uint16_t interval;      /**< Connection interval. Time = Value x 1.25 ms */
  uint16_t latency;       /**< Peripheral latency (how many connection intervals
                               the peripheral can skip) */
  uint16_t timeout;       /**< Supervision timeout. Time = Value x 10 ms */
  uint8_t  security_mode; /**< Enum @ref sl_bt_connection_security_t. Connection
                               security mode. Values:
                                 - <b>sl_bt_connection_mode1_level1 (0x0):</b>
                                   No security
                                 - <b>sl_bt_connection_mode1_level2 (0x1):</b>
                                   Unauthenticated pairing with encryption
                                 - <b>sl_bt_connection_mode1_level3 (0x2):</b>
                                   Authenticated pairing with encryption
                                 - <b>sl_bt_connection_mode1_level4 (0x3):</b>
                                   Authenticated Secure Connections pairing with
                                   encryption using a 128-bit strength
                                   encryption key */
  uint16_t txsize;        /**< Maximum Data Channel PDU Payload size that the
                               controller can send in an air packet */
});

typedef struct sl_bt_evt_connection_parameters_s sl_bt_evt_connection_parameters_t;

/** @} */ // end addtogroup sl_bt_evt_connection_parameters

/**
 * @addtogroup sl_bt_evt_connection_phy_status sl_bt_evt_connection_phy_status
 * @{
 * @brief Indicates that PHY update procedure is completed.
 */

/** @brief Identifier of the phy_status event */
#define sl_bt_evt_connection_phy_status_id                           0x040600a0

/***************************************************************************//**
 * @brief Data structure of the phy_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_connection_phy_status_s
{
  uint8_t connection; /**< Connection handle */
  uint8_t phy;        /**< Current active PHY. See values from @ref
                           sl_bt_connection_set_preferred_phy command. */
});

typedef struct sl_bt_evt_connection_phy_status_s sl_bt_evt_connection_phy_status_t;

/** @} */ // end addtogroup sl_bt_evt_connection_phy_status

/**
 * @addtogroup sl_bt_evt_connection_rssi sl_bt_evt_connection_rssi
 * @{
 * @brief Triggered when an connection_get_rssi command has completed.
 */

/** @brief Identifier of the rssi event */
#define sl_bt_evt_connection_rssi_id                                 0x030600a0

/***************************************************************************//**
 * @brief Data structure of the rssi event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_connection_rssi_s
{
  uint8_t connection; /**< Connection handle */
  uint8_t status;     /**< Command complete status:
                             - <b>0x00:</b> The command succeeded
                             - <b>0x01-0xFF:</b> The command failed. See
                               Bluetooth Core specification v5.0 [Vol 2] Part D,
                               Error Codes */
  int8_t  rssi;       /**< The median of the last seven measured RSSI values on
                           the connection. Units: dBm. Range: -127 to +20.
                           Ignore this parameter if the command fails. */
});

typedef struct sl_bt_evt_connection_rssi_s sl_bt_evt_connection_rssi_t;

/** @} */ // end addtogroup sl_bt_evt_connection_rssi

/**
 * @addtogroup sl_bt_evt_connection_get_remote_tx_power_completed sl_bt_evt_connection_get_remote_tx_power_completed
 * @{
 * @brief Indicates that reading remote transmit power initiated by @ref
 * sl_bt_connection_get_remote_tx_power command has completed
 */

/** @brief Identifier of the get_remote_tx_power_completed event */
#define sl_bt_evt_connection_get_remote_tx_power_completed_id        0x050600a0

/***************************************************************************//**
 * @brief Data structure of the get_remote_tx_power_completed event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_connection_get_remote_tx_power_completed_s
{
  uint16_t status;      /**< SL_STATUS_OK or another error code indicating the
                             reading remote transmit power operation failed. */
  uint8_t  connection;  /**< Handle of the connection */
  uint8_t  phy;         /**< Enum @ref sl_bt_gap_phy_coding_t. The PHY for which
                             the transmit power is reported. Values:
                               - <b>sl_bt_gap_phy_coding_1m_uncoded (0x1):</b>
                                 1M PHY
                               - <b>sl_bt_gap_phy_coding_2m_uncoded (0x2):</b>
                                 2M PHY
                               - <b>sl_bt_gap_phy_coding_125k_coded (0x4):</b>
                                 125k Coded PHY (S=8)
                               - <b>sl_bt_gap_phy_coding_500k_coded (0x8):</b>
                                 500k Coded PHY (S=2) */
  int8_t   power_level; /**< Transmit power level. Values:
                               - <b>Range -127 to 20:</b> The transmit power
                                 level in dBm
                               - <b>@ref SL_BT_CONNECTION_TX_POWER_UNMANAGED
                                 (0x7E):</b> Remote device is not managing power
                                 levels on this PHY.
                               - <b>@ref SL_BT_CONNECTION_TX_POWER_UNAVAILABLE
                                 (0x7F):</b> Transmit power level is not
                                 available. */
  uint8_t  flags;       /**< Enum @ref sl_bt_connection_tx_power_flag_t.
                             Transmit power level flags. Ignore this field if @p
                             power_level is set to 0x7E or 0x7F. Values:
                               - <b>sl_bt_connection_tx_power_at_minimum
                                 (0x1):</b> Transmit power level is at minimum
                                 level.
                               - <b>sl_bt_connection_tx_power_at_maximum
                                 (0x2):</b> Transmit power level is at maximum
                                 level. */
  int8_t   delta;       /**< Change in transmit power level. Values:
                               - <b>@ref
                                 SL_BT_CONNECTION_TX_POWER_CHANGE_UNAVAILABLE
                                 (0x7F):</b> Change is not available or is out
                                 of range.
                               - <b>Other values:</b> positive indicates
                                 increased power, negative indicates decreased
                                 power, and zero indicates unchanged. Units: dB */
});

typedef struct sl_bt_evt_connection_get_remote_tx_power_completed_s sl_bt_evt_connection_get_remote_tx_power_completed_t;

/** @} */ // end addtogroup sl_bt_evt_connection_get_remote_tx_power_completed

/**
 * @addtogroup sl_bt_evt_connection_tx_power sl_bt_evt_connection_tx_power
 * @{
 * @brief Reports a transmit power change on the local device of a connection if
 * transmit power reporting has been enabled
 *
 * Enable or disable transmit power reporting using @ref
 * sl_bt_connection_set_power_reporting command.
 */

/** @brief Identifier of the tx_power event */
#define sl_bt_evt_connection_tx_power_id                             0x060600a0

/***************************************************************************//**
 * @brief Data structure of the tx_power event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_connection_tx_power_s
{
  uint8_t connection;  /**< Handle of the connection */
  uint8_t phy;         /**< Enum @ref sl_bt_gap_phy_coding_t. The PHY which the
                            transmit power is reported for. Values:
                              - <b>sl_bt_gap_phy_coding_1m_uncoded (0x1):</b> 1M
                                PHY
                              - <b>sl_bt_gap_phy_coding_2m_uncoded (0x2):</b> 2M
                                PHY
                              - <b>sl_bt_gap_phy_coding_125k_coded (0x4):</b>
                                125k Coded PHY (S=8)
                              - <b>sl_bt_gap_phy_coding_500k_coded (0x8):</b>
                                500k Coded PHY (S=2) */
  int8_t  power_level; /**< Transmit power level. Values:
                              - <b>Range -127 to 20:</b> The transmit power
                                level in dBm
                              - <b>@ref SL_BT_CONNECTION_TX_POWER_UNAVAILABLE
                                (0x7F):</b> Transmit power level is not
                                available. */
  uint8_t flags;       /**< Enum @ref sl_bt_connection_tx_power_flag_t. Transmit
                            power level flags. Ignore this field if @p
                            power_level is set to 0x7E or 0x7F. Values:
                              - <b>sl_bt_connection_tx_power_at_minimum
                                (0x1):</b> Transmit power level is at minimum
                                level.
                              - <b>sl_bt_connection_tx_power_at_maximum
                                (0x2):</b> Transmit power level is at maximum
                                level. */
  int8_t  delta;       /**< Change in transmit power level. Values:
                              - <b>@ref
                                SL_BT_CONNECTION_TX_POWER_CHANGE_UNAVAILABLE
                                (0x7F):</b> Change is not available or is out of
                                range.
                              - <b>Other values:</b> positive indicates
                                increased power, negative indicates decreased
                                power, and zero indicates unchanged. Units: dB */
});

typedef struct sl_bt_evt_connection_tx_power_s sl_bt_evt_connection_tx_power_t;

/** @} */ // end addtogroup sl_bt_evt_connection_tx_power

/**
 * @addtogroup sl_bt_evt_connection_remote_tx_power sl_bt_evt_connection_remote_tx_power
 * @{
 * @brief Reports a transmit power change on the remote device of a connection
 * if transmit power reporting has been enabled
 *
 * Enable or disable transmit power reporting using @ref
 * sl_bt_connection_set_power_reporting command.
 */

/** @brief Identifier of the remote_tx_power event */
#define sl_bt_evt_connection_remote_tx_power_id                      0x070600a0

/***************************************************************************//**
 * @brief Data structure of the remote_tx_power event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_connection_remote_tx_power_s
{
  uint8_t connection;  /**< Handle of the connection */
  uint8_t phy;         /**< Enum @ref sl_bt_gap_phy_coding_t. The PHY which the
                            transmit power is reported for. Values:
                              - <b>sl_bt_gap_phy_coding_1m_uncoded (0x1):</b> 1M
                                PHY
                              - <b>sl_bt_gap_phy_coding_2m_uncoded (0x2):</b> 2M
                                PHY
                              - <b>sl_bt_gap_phy_coding_125k_coded (0x4):</b>
                                125k Coded PHY (S=8)
                              - <b>sl_bt_gap_phy_coding_500k_coded (0x8):</b>
                                500k Coded PHY (S=2) */
  int8_t  power_level; /**< Transmit power level. Values:
                              - <b>Range -127 to 20:</b> The transmit power
                                level in dBm
                              - <b>@ref SL_BT_CONNECTION_TX_POWER_UNMANAGED
                                (0x7E):</b> Remote device is not managing power
                                levels on this PHY.
                              - <b>@ref SL_BT_CONNECTION_TX_POWER_UNAVAILABLE
                                (0x7F):</b> Transmit power level is not
                                available. */
  uint8_t flags;       /**< Enum @ref sl_bt_connection_tx_power_flag_t. Transmit
                            power level flags. Ignore this field if @p
                            power_level is set to 0x7E or 0x7F. Values:
                              - <b>sl_bt_connection_tx_power_at_minimum
                                (0x1):</b> Transmit power level is at minimum
                                level.
                              - <b>sl_bt_connection_tx_power_at_maximum
                                (0x2):</b> Transmit power level is at maximum
                                level. */
  int8_t  delta;       /**< Change in transmit power level. Values:
                              - <b>@ref
                                SL_BT_CONNECTION_TX_POWER_CHANGE_UNAVAILABLE
                                (0x7F):</b> Change is not available or is out of
                                range.
                              - <b>Other values:</b> positive indicates
                                increased power, negative indicates decreased
                                power, and zero indicates unchanged. Units: dB */
});

typedef struct sl_bt_evt_connection_remote_tx_power_s sl_bt_evt_connection_remote_tx_power_t;

/** @} */ // end addtogroup sl_bt_evt_connection_remote_tx_power

/**
 * @addtogroup sl_bt_evt_connection_closed sl_bt_evt_connection_closed
 * @{
 * @brief Indicates that a connection was closed.
 */

/** @brief Identifier of the closed event */
#define sl_bt_evt_connection_closed_id                               0x010600a0

/***************************************************************************//**
 * @brief Data structure of the closed event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_connection_closed_s
{
  uint16_t reason;     /**< Reason of connection close */
  uint8_t  connection; /**< Handle of the closed connection */
});

typedef struct sl_bt_evt_connection_closed_s sl_bt_evt_connection_closed_t;

/** @} */ // end addtogroup sl_bt_evt_connection_closed

/**
 * @addtogroup sl_bt_evt_connection_remote_used_features sl_bt_evt_connection_remote_used_features
 * @{
 * @brief List of link layer features supported by the remote device.
 */

/** @brief Identifier of the remote_used_features event */
#define sl_bt_evt_connection_remote_used_features_id                 0x080600a0

/***************************************************************************//**
 * @brief Data structure of the remote_used_features event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_connection_remote_used_features_s
{
  uint8_t    connection; /**< Connection handle */
  uint8array features;   /**< This parameter is 8 bytes in little endian format
                              and contains bit fields describing the supported
                              link layer features of the remote device. Bit
                              value 1 means that the feature is supported.
                              The bits are explained in Bluetooth specification
                              Vol 6, Part B, 4.6. */
});

typedef struct sl_bt_evt_connection_remote_used_features_s sl_bt_evt_connection_remote_used_features_t;

/** @} */ // end addtogroup sl_bt_evt_connection_remote_used_features

/***************************************************************************//**
 *
 * Set the default Bluetooth connection parameters. The values are valid for all
 * subsequent connections initiated by this device. To change parameters of an
 * already established connection, use the command @ref
 * sl_bt_connection_set_parameters.
 *
 * @param[in] min_interval @parblock
 *   Minimum value for the connection event interval. This must be set less than
 *   or equal to @p max_interval.
 *     - Time = Value x 1.25 ms
 *     - Range: 0x0006 to 0x0c80
 *     - Time Range: 7.5 ms to 4 s
 *
 *   Default value: 20 ms
 *   @endparblock
 * @param[in] max_interval @parblock
 *   Maximum value for the connection event interval. This must be set greater
 *   than or equal to @p min_interval.
 *     - Time = Value x 1.25 ms
 *     - Range: 0x0006 to 0x0c80
 *     - Time Range: 7.5 ms to 4 s
 *
 *   Default value: 50 ms
 *   @endparblock
 * @param[in] latency @parblock
 *   Peripheral latency, which defines how many connection intervals the
 *   peripheral can skip if it has no data to send
 *     - Range: 0x0000 to 0x01f4
 *
 *   Default value: 0
 *   @endparblock
 * @param[in] timeout @parblock
 *   Supervision timeout, which defines the time that the connection is
 *   maintained although the devices can't communicate at the currently
 *   configured connection intervals.
 *     - Range: 0x000a to 0x0c80
 *     - Time = Value x 10 ms
 *     - Time Range: 100 ms to 32 s
 *     - The value in milliseconds must be larger than (1 + @p latency) * @p
 *       max_interval * 2, where @p max_interval is given in milliseconds
 *
 *   Set the supervision timeout at a value which allows communication attempts
 *   over at least a few connection intervals.
 *
 *   Default value: 1000 ms
 *   @endparblock
 * @param[in] min_ce_length @parblock
 *   Minimum value for the connection event length. This must be set be less
 *   than or equal to @p max_ce_length.
 *     - Time = Value x 0.625 ms
 *     - Range: 0x0000 to 0xffff
 *
 *   Default value: 0x0000
 *
 *   Value is not currently used and is reserved for future. Set to 0.
 *   @endparblock
 * @param[in] max_ce_length @parblock
 *   Maximum value for the connection event length. This must be set greater
 *   than or equal to @p min_ce_length.
 *     - Time = Value x 0.625 ms
 *     - Range: 0x0000 to 0xffff
 *
 *   Default value: 0xffff
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_set_default_parameters(uint16_t min_interval,
                                                    uint16_t max_interval,
                                                    uint16_t latency,
                                                    uint16_t timeout,
                                                    uint16_t min_ce_length,
                                                    uint16_t max_ce_length);

/***************************************************************************//**
 *
 * Set default preferred and accepted PHYs. PHY settings will be used for all
 * subsequent connections. Non-preferred PHY can also be set if the remote
 * device does not accept any of the preferred PHYs.
 *
 * The parameter @p accepted_phy is used to specify PHYs that the stack can
 * accept in a remotely-initiated PHY update request. A PHY update will not
 * happen if none of the accepted PHYs are present in the request.
 *
 * <b>NOTE:</b> 2M and Coded PHYs are not supported by all devices.
 *
 * @param[in] preferred_phy @parblock
 *   Preferred PHYs. This parameter is a bitfield and multiple PHYs can be set.
 *     - <b>0x01:</b> 1M PHY
 *     - <b>0x02:</b> 2M PHY
 *     - <b>0x04:</b> Coded PHY
 *     - <b>0xff:</b> Any PHYs
 *
 *   Default: 0xff (no preference)
 *   @endparblock
 * @param[in] accepted_phy @parblock
 *   Accepted PHYs in remotely-initiated PHY update request. This parameter is a
 *   bitfield and multiple PHYs can be set.
 *     - <b>0x01:</b> 1M PHY
 *     - <b>0x02:</b> 2M PHY
 *     - <b>0x04:</b> Coded PHY
 *     - <b>0xff:</b> Any PHYs
 *
 *   Default: 0xff (all PHYs accepted)
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_set_default_preferred_phy(uint8_t preferred_phy,
                                                       uint8_t accepted_phy);

/***************************************************************************//**
 *
 * Connect to an advertising device with the specified initiating PHY on which
 * connectable advertisements on primary advertising channels are received. The
 * Bluetooth stack will enter a state where it continuously scans for the
 * connectable advertising packets from the remote device, which matches the
 * Bluetooth address given as a parameter. Scan parameters set in @ref
 * sl_bt_scanner_set_timing are used in this operation. Upon receiving the
 * advertising packet, the module will send a connection request packet to the
 * target device to initiate a Bluetooth connection. To cancel an ongoing
 * connection process, use @ref sl_bt_connection_close command with the handle
 * received in response from this command.
 *
 * A connection is opened in no-security mode. If the GATT client needs to read
 * or write the attributes on GATT server requiring encryption or
 * authentication, it must first encrypt the connection using an appropriate
 * authentication method.
 *
 * If a connection can't be established, for example, the remote device has gone
 * out of range, has entered into deep sleep, or is not advertising, the stack
 * will try to connect forever. In this case, the application will not get an
 * event related to the connection request. To recover from this situation, the
 * application can implement a timeout and call @ref sl_bt_connection_close to
 * cancel the connection request.
 *
 * This command fails with the connection limit exceeded error if the number of
 * connections attempted exceeds the configured MAX_CONNECTIONS value.
 *
 * This command fails with the invalid parameter error if the initiating PHY
 * value is invalid or the device does not support PHY.
 *
 * Subsequent calls of this command have to wait for the ongoing command to
 * complete. A received event @ref sl_bt_evt_connection_opened indicates that
 * the connection opened successfully and a received event @ref
 * sl_bt_evt_connection_closed indicates that connection failures have occurred.
 *
 * @param[in] address Address of the device to connect to
 * @param[in] address_type Enum @ref sl_bt_gap_address_type_t. Address type of
 *   the device to connect to. Values:
 *     - <b>sl_bt_gap_public_address (0x0):</b> Public device address
 *     - <b>sl_bt_gap_static_address (0x1):</b> Static device address
 *     - <b>sl_bt_gap_random_resolvable_address (0x2):</b> Resolvable private
 *       random address
 *     - <b>sl_bt_gap_random_nonresolvable_address (0x3):</b> Non-resolvable
 *       private random address
 * @param[in] initiating_phy Enum @ref sl_bt_gap_phy_t. The initiating PHY.
 *   Values:
 *     - <b>sl_bt_gap_phy_1m (0x1):</b> 1M PHY
 *     - <b>sl_bt_gap_phy_coded (0x4):</b> Coded PHY, 125k (S=8) or 500k (S=2)
 * @param[out] connection Handle that will be assigned to the connection after
 *   the connection is established. This handle is valid only if the result code
 *   of this response is 0 (zero).
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_opened - This event is triggered after the
 *     connection is opened and indicates whether the devices are already bonded
 *     and the role of the device in this connection.
 *   - @ref sl_bt_evt_connection_parameters - This event indicates the
 *     connection parameters and security mode of the connection.
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_open(bd_addr address,
                                  uint8_t address_type,
                                  uint8_t initiating_phy,
                                  uint8_t *connection);

/***************************************************************************//**
 *
 * Request a change in the connection parameters of a Bluetooth connection.
 *
 * @param[in] connection Connection Handle
 * @param[in] min_interval Minimum value for the connection event interval. This
 *   must be set less than or equal to @p max_interval.
 *     - Time = Value x 1.25 ms
 *     - Range: 0x0006 to 0x0c80
 *     - Time Range: 7.5 ms to 4 s
 * @param[in] max_interval Maximum value for the connection event interval. This
 *   must be set greater than or equal to @p min_interval.
 *     - Time = Value x 1.25 ms
 *     - Range: 0x0006 to 0x0c80
 *     - Time Range: 7.5 ms to 4 s
 * @param[in] latency @parblock
 *   Peripheral latency, which defines how many connection intervals the
 *   peripheral can skip if it has no data to send
 *     - Range: 0x0000 to 0x01f4
 *
 *   Use 0x0000 for default value
 *   @endparblock
 * @param[in] timeout @parblock
 *   Supervision timeout, which defines the time that the connection is
 *   maintained although the devices can't communicate at the currently
 *   configured connection intervals.
 *     - Range: 0x000a to 0x0c80
 *     - Time = Value x 10 ms
 *     - Time Range: 100 ms to 32 s
 *     - The value in milliseconds must be larger than (1 + @p latency) * @p
 *       max_interval * 2, where @p max_interval is given in milliseconds
 *
 *   Set the supervision timeout at a value which allows communication attempts
 *   over at least a few connection intervals.
 *   @endparblock
 * @param[in] min_ce_length @parblock
 *   Minimum value for the connection event length. This must be set less than
 *   or equal to @p max_ce_length.
 *     - Time = Value x 0.625 ms
 *     - Range: 0x0000 to 0xffff
 *
 *   Value is not currently used and is reserved for future. Set to 0.
 *   @endparblock
 * @param[in] max_ce_length @parblock
 *   Maximum value for the connection event length. This must be set greater
 *   than or equal to @p min_ce_length.
 *     - Time = Value x 0.625 ms
 *     - Range: 0x0000 to 0xffff
 *
 *   Use 0xffff for no limitation.
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_parameters - Triggered after new connection
 *     parameters are applied on the connection.
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_set_parameters(uint8_t connection,
                                            uint16_t min_interval,
                                            uint16_t max_interval,
                                            uint16_t latency,
                                            uint16_t timeout,
                                            uint16_t min_ce_length,
                                            uint16_t max_ce_length);

/***************************************************************************//**
 *
 * Sets preferred and accepted PHYs for the given connection. Event @ref
 * sl_bt_evt_connection_phy_status is received when PHY update procedure is
 * completed. Non-preferred PHY can also be set if remote device does not accept
 * any of the preferred PHYs.
 *
 * The parameter @p accepted_phy is used for specifying the PHYs that the stack
 * can accept in a remote initiated PHY update request. A PHY update will not
 * occur if none of the accepted PHYs presents in the request.
 *
 * <b>NOTE:</b> 2M and Coded PHYs are not supported by all devices.
 *
 * @param[in] connection Connection handle
 * @param[in] preferred_phy @parblock
 *   Preferred PHYs. This parameter is a bitfield and multiple PHYs can be set.
 *     - <b>0x01:</b> 1M PHY
 *     - <b>0x02:</b> 2M PHY
 *     - <b>0x04:</b> 125k Coded PHY (S=8)
 *     - <b>0x08:</b> 500k Coded PHY (S=2)
 *
 *   Default: 0xff (no preference)
 *   @endparblock
 * @param[in] accepted_phy @parblock
 *   Accepted PHYs in remotely-initiated PHY update requests. This parameter is
 *   a bitfield and multiple PHYs can be set.
 *     - <b>0x01:</b> 1M PHY
 *     - <b>0x02:</b> 2M PHY
 *     - <b>0x04:</b> Coded PHY
 *     - <b>0xff:</b> Any PHYs
 *
 *   Default: 0xff (all PHYs accepted)
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_phy_status
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_set_preferred_phy(uint8_t connection,
                                               uint8_t preferred_phy,
                                               uint8_t accepted_phy);

/***************************************************************************//**
 *
 * Temporarily enable or disable peripheral latency. Used only when Bluetooth
 * device is acting as peripheral. When peripheral latency is disabled, the
 * peripheral latency connection parameter is not set to 0 but the device will
 * wake up on every connection interval to receive and send packets.
 *
 * @param[in] connection Connection Handle
 * @param[in] disable 0 enable, 1 disable peripheral latency. Default: 0
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_disable_slave_latency(uint8_t connection,
                                                   uint8_t disable);

/***************************************************************************//**
 *
 * Get the latest RSSI value of a Bluetooth connection. The RSSI value will be
 * reported in a @ref sl_bt_evt_connection_rssi event.
 *
 * @param[in] connection Connection handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_rssi - Triggered when this command has
 *     completed.
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_get_rssi(uint8_t connection);

/***************************************************************************//**
 *
 * Read channel map for a specified connection.
 *
 * @param[in] connection Connection Handle
 * @param[in] max_channel_map_size Size of output buffer passed in @p
 *   channel_map
 * @param[out] channel_map_len On return, set to the length of output data
 *   written to @p channel_map
 * @param[out] channel_map @parblock
 *   This parameter is 5 bytes and contains 37 1-bit fields.
 *   The nth field (in the range 0 to 36) contains the value for the link layer
 *   channel index n.
 *     - <b>0:</b> Channel n is unused.
 *     - <b>1:</b> Channel n is used.
 *
 *   The most significant bits are reserved for future use.
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_read_channel_map(uint8_t connection,
                                              size_t max_channel_map_size,
                                              size_t *channel_map_len,
                                              uint8_t *channel_map);

/***************************************************************************//**
 *
 * Enable or disable the transmit power reporting for the local device on a
 * connection. When transmit power reporting is enabled, event @ref
 * sl_bt_evt_connection_tx_power is generated when transmit power on the local
 * device changes.
 *
 * By default power reporting for local device is enabled.
 *
 * @param[in] connection Handle of the connection
 * @param[in] mode Enum @ref sl_bt_connection_power_reporting_mode_t. Transmit
 *   power reporting mode. Values:
 *     - <b>sl_bt_connection_power_reporting_disable (0x0):</b> Disable transmit
 *       power reporting
 *     - <b>sl_bt_connection_power_reporting_enable (0x1):</b> Enable transmit
 *       power reporting
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_tx_power
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_set_power_reporting(uint8_t connection,
                                                 uint8_t mode);

/***************************************************************************//**
 *
 * Enable or disable the transmit power reporting for the remote device on a
 * connection. When transmit power reporting is enabled, event @ref
 * sl_bt_evt_connection_remote_tx_power is generated when transmit power on the
 * remote device changes.
 *
 * By default, power reporting for the remote device is disabled.
 *
 * @param[in] connection Handle of the connection
 * @param[in] mode Enum @ref sl_bt_connection_power_reporting_mode_t. Transmit
 *   power reporting mode. Values:
 *     - <b>sl_bt_connection_power_reporting_disable (0x0):</b> Disable transmit
 *       power reporting
 *     - <b>sl_bt_connection_power_reporting_enable (0x1):</b> Enable transmit
 *       power reporting
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_remote_tx_power
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_set_remote_power_reporting(uint8_t connection,
                                                        uint8_t mode);

/***************************************************************************//**
 *
 * Get the transmit power of the local device on the given connection and PHY.
 *
 * @param[in] connection Handle of the connection
 * @param[in] phy Enum @ref sl_bt_gap_phy_coding_t. The PHY. Values:
 *     - <b>sl_bt_gap_phy_coding_1m_uncoded (0x1):</b> 1M PHY
 *     - <b>sl_bt_gap_phy_coding_2m_uncoded (0x2):</b> 2M PHY
 *     - <b>sl_bt_gap_phy_coding_125k_coded (0x4):</b> 125k Coded PHY (S=8)
 *     - <b>sl_bt_gap_phy_coding_500k_coded (0x8):</b> 500k Coded PHY (S=2)
 * @param[out] current_level The current transmit power level of the PHY on the
 *   connection. Values:
 *     - <b>Range -127 to 20:</b> The transmit power level in dBm
 *     - <b>@ref SL_BT_CONNECTION_TX_POWER_UNAVAILABLE (0x7F):</b> Transmit
 *       power level is not available.
 * @param[out] max_level The maximum transmit power level of the PHY on the
 *   connection. Values: Range -127 to +20 in dBm.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_get_tx_power(uint8_t connection,
                                          uint8_t phy,
                                          int8_t *current_level,
                                          int8_t *max_level);

/***************************************************************************//**
 *
 * Get the transmit power of the remote device on the given connection and PHY.
 * Transmit power levels are returned in event @ref
 * sl_bt_evt_connection_get_remote_tx_power_completed after the operation
 * completed.
 *
 * @param[in] connection Handle of the connection
 * @param[in] phy Enum @ref sl_bt_gap_phy_coding_t. The PHY. Values:
 *     - <b>sl_bt_gap_phy_coding_1m_uncoded (0x1):</b> 1M PHY
 *     - <b>sl_bt_gap_phy_coding_2m_uncoded (0x2):</b> 2M PHY
 *     - <b>sl_bt_gap_phy_coding_125k_coded (0x4):</b> 125k Coded PHY (S=8)
 *     - <b>sl_bt_gap_phy_coding_500k_coded (0x8):</b> 500k Coded PHY (S=2)
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_get_remote_tx_power_completed
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_get_remote_tx_power(uint8_t connection,
                                                 uint8_t phy);

/***************************************************************************//**
 *
 * Close a Bluetooth connection or cancel an ongoing connection establishment
 * process. The parameter is a connection handle which is reported in @ref
 * sl_bt_evt_connection_opened event or @ref sl_bt_connection_open command
 * response.
 *
 * @param[in] connection Handle of the connection to be closed
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_closed
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_close(uint8_t connection);

/***************************************************************************//**
 *
 * Read link layer features supported by the remote device.
 *
 * @param[in] connection Connection Handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_remote_used_features
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_read_remote_used_features(uint8_t connection);

/** @} */ // end addtogroup sl_bt_connection

/**
 * @addtogroup sl_bt_gatt GATT Client
 * @{
 *
 * @brief GATT Client
 *
 * The commands and events in this class are used to browse and manage
 * attributes in a remote GATT server.
 */

/* Command and Response IDs */
#define sl_bt_cmd_gatt_set_max_mtu_id                                0x00090020
#define sl_bt_cmd_gatt_discover_primary_services_id                  0x01090020
#define sl_bt_cmd_gatt_discover_primary_services_by_uuid_id          0x02090020
#define sl_bt_cmd_gatt_find_included_services_id                     0x10090020
#define sl_bt_cmd_gatt_discover_characteristics_id                   0x03090020
#define sl_bt_cmd_gatt_discover_characteristics_by_uuid_id           0x04090020
#define sl_bt_cmd_gatt_discover_descriptors_id                       0x06090020
#define sl_bt_cmd_gatt_set_characteristic_notification_id            0x05090020
#define sl_bt_cmd_gatt_send_characteristic_confirmation_id           0x0d090020
#define sl_bt_cmd_gatt_read_characteristic_value_id                  0x07090020
#define sl_bt_cmd_gatt_read_characteristic_value_from_offset_id      0x12090020
#define sl_bt_cmd_gatt_read_multiple_characteristic_values_id        0x11090020
#define sl_bt_cmd_gatt_read_characteristic_value_by_uuid_id          0x08090020
#define sl_bt_cmd_gatt_write_characteristic_value_id                 0x09090020
#define sl_bt_cmd_gatt_write_characteristic_value_without_response_id 0x0a090020
#define sl_bt_cmd_gatt_prepare_characteristic_value_write_id         0x0b090020
#define sl_bt_cmd_gatt_prepare_characteristic_value_reliable_write_id 0x13090020
#define sl_bt_cmd_gatt_execute_characteristic_value_write_id         0x0c090020
#define sl_bt_cmd_gatt_read_descriptor_value_id                      0x0e090020
#define sl_bt_cmd_gatt_write_descriptor_value_id                     0x0f090020
#define sl_bt_rsp_gatt_set_max_mtu_id                                0x00090020
#define sl_bt_rsp_gatt_discover_primary_services_id                  0x01090020
#define sl_bt_rsp_gatt_discover_primary_services_by_uuid_id          0x02090020
#define sl_bt_rsp_gatt_find_included_services_id                     0x10090020
#define sl_bt_rsp_gatt_discover_characteristics_id                   0x03090020
#define sl_bt_rsp_gatt_discover_characteristics_by_uuid_id           0x04090020
#define sl_bt_rsp_gatt_discover_descriptors_id                       0x06090020
#define sl_bt_rsp_gatt_set_characteristic_notification_id            0x05090020
#define sl_bt_rsp_gatt_send_characteristic_confirmation_id           0x0d090020
#define sl_bt_rsp_gatt_read_characteristic_value_id                  0x07090020
#define sl_bt_rsp_gatt_read_characteristic_value_from_offset_id      0x12090020
#define sl_bt_rsp_gatt_read_multiple_characteristic_values_id        0x11090020
#define sl_bt_rsp_gatt_read_characteristic_value_by_uuid_id          0x08090020
#define sl_bt_rsp_gatt_write_characteristic_value_id                 0x09090020
#define sl_bt_rsp_gatt_write_characteristic_value_without_response_id 0x0a090020
#define sl_bt_rsp_gatt_prepare_characteristic_value_write_id         0x0b090020
#define sl_bt_rsp_gatt_prepare_characteristic_value_reliable_write_id 0x13090020
#define sl_bt_rsp_gatt_execute_characteristic_value_write_id         0x0c090020
#define sl_bt_rsp_gatt_read_descriptor_value_id                      0x0e090020
#define sl_bt_rsp_gatt_write_descriptor_value_id                     0x0f090020

/**
 * @brief 
                These values indicate which attribute request or response has caused the event.
            
 */
typedef enum
{
  sl_bt_gatt_read_by_type_request      = 0x8,  /**< (0x8) Read by type request */
  sl_bt_gatt_read_by_type_response     = 0x9,  /**< (0x9) Read by type response */
  sl_bt_gatt_read_request              = 0xa,  /**< (0xa) Read request */
  sl_bt_gatt_read_response             = 0xb,  /**< (0xb) Read response */
  sl_bt_gatt_read_blob_request         = 0xc,  /**< (0xc) Read blob request */
  sl_bt_gatt_read_blob_response        = 0xd,  /**< (0xd) Read blob response */
  sl_bt_gatt_read_multiple_request     = 0xe,  /**< (0xe) Read multiple request */
  sl_bt_gatt_read_multiple_response    = 0xf,  /**< (0xf) Read multiple response */
  sl_bt_gatt_write_request             = 0x12, /**< (0x12) Write request */
  sl_bt_gatt_write_response            = 0x13, /**< (0x13) Write response */
  sl_bt_gatt_write_command             = 0x52, /**< (0x52) Write command */
  sl_bt_gatt_prepare_write_request     = 0x16, /**< (0x16) Prepare write request */
  sl_bt_gatt_prepare_write_response    = 0x17, /**< (0x17) Prepare write
                                                    response */
  sl_bt_gatt_execute_write_request     = 0x18, /**< (0x18) Execute write request */
  sl_bt_gatt_execute_write_response    = 0x19, /**< (0x19) Execute write
                                                    response */
  sl_bt_gatt_handle_value_notification = 0x1b, /**< (0x1b) Notification */
  sl_bt_gatt_handle_value_indication   = 0x1d  /**< (0x1d) Indication */
} sl_bt_gatt_att_opcode_t;

/**
 * @brief 
                These values define whether the client is to receive notifications or indications from a remote GATT
                server.
            
 */
typedef enum
{
  sl_bt_gatt_disable      = 0x0, /**< (0x0) Disable notifications and
                                      indications */
  sl_bt_gatt_notification = 0x1, /**< (0x1) Notification */
  sl_bt_gatt_indication   = 0x2  /**< (0x2) Indication */
} sl_bt_gatt_client_config_flag_t;

/**
 * @brief 
                These values define whether the GATT server is to cancel all queued writes or commit all queued writes
                to a remote database.
            
 */
typedef enum
{
  sl_bt_gatt_cancel = 0x0, /**< (0x0) Cancel all queued writes */
  sl_bt_gatt_commit = 0x1  /**< (0x1) Commit all queued writes */
} sl_bt_gatt_execute_write_flag_t;

/**
 * @addtogroup sl_bt_evt_gatt_mtu_exchanged sl_bt_evt_gatt_mtu_exchanged
 * @{
 * @brief Indicates that an ATT_MTU exchange procedure is completed
 *
 * The @p mtu parameter describes new MTU size. MTU size 23 is used before this
 * event is received.
 */

/** @brief Identifier of the mtu_exchanged event */
#define sl_bt_evt_gatt_mtu_exchanged_id                              0x000900a0

/***************************************************************************//**
 * @brief Data structure of the mtu_exchanged event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_mtu_exchanged_s
{
  uint8_t  connection; /**< Connection handle */
  uint16_t mtu;        /**< Exchanged ATT_MTU */
});

typedef struct sl_bt_evt_gatt_mtu_exchanged_s sl_bt_evt_gatt_mtu_exchanged_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_mtu_exchanged

/**
 * @addtogroup sl_bt_evt_gatt_service sl_bt_evt_gatt_service
 * @{
 * @brief Indicate that a GATT service in the remote GATT database was
 * discovered
 *
 * This event is generated after issuing either the @ref
 * sl_bt_gatt_discover_primary_services or @ref
 * sl_bt_gatt_discover_primary_services_by_uuid command.
 */

/** @brief Identifier of the service event */
#define sl_bt_evt_gatt_service_id                                    0x010900a0

/***************************************************************************//**
 * @brief Data structure of the service event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_service_s
{
  uint8_t    connection; /**< Connection handle */
  uint32_t   service;    /**< GATT service handle */
  uint8array uuid;       /**< Service UUID in little endian format */
});

typedef struct sl_bt_evt_gatt_service_s sl_bt_evt_gatt_service_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_service

/**
 * @addtogroup sl_bt_evt_gatt_characteristic sl_bt_evt_gatt_characteristic
 * @{
 * @brief Indicates that a GATT characteristic in the remote GATT database was
 * discovered
 *
 * This event is generated after issuing either the @ref
 * sl_bt_gatt_discover_characteristics or @ref
 * sl_bt_gatt_discover_characteristics_by_uuid command.
 */

/** @brief Identifier of the characteristic event */
#define sl_bt_evt_gatt_characteristic_id                             0x020900a0

/***************************************************************************//**
 * @brief Data structure of the characteristic event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_characteristic_s
{
  uint8_t    connection;     /**< Connection handle */
  uint16_t   characteristic; /**< GATT characteristic handle */
  uint8_t    properties;     /**< Characteristic properties */
  uint8array uuid;           /**< Characteristic UUID in little endian format */
});

typedef struct sl_bt_evt_gatt_characteristic_s sl_bt_evt_gatt_characteristic_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_characteristic

/**
 * @addtogroup sl_bt_evt_gatt_descriptor sl_bt_evt_gatt_descriptor
 * @{
 * @brief Indicates that a GATT characteristic descriptor in the remote GATT
 * database was discovered
 *
 * It is generated after issuing the @ref sl_bt_gatt_discover_descriptors
 * command.
 */

/** @brief Identifier of the descriptor event */
#define sl_bt_evt_gatt_descriptor_id                                 0x030900a0

/***************************************************************************//**
 * @brief Data structure of the descriptor event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_descriptor_s
{
  uint8_t    connection; /**< Connection handle */
  uint16_t   descriptor; /**< GATT characteristic descriptor handle */
  uint8array uuid;       /**< Descriptor UUID in little endian format */
});

typedef struct sl_bt_evt_gatt_descriptor_s sl_bt_evt_gatt_descriptor_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_descriptor

/**
 * @addtogroup sl_bt_evt_gatt_characteristic_value sl_bt_evt_gatt_characteristic_value
 * @{
 * @brief Indicates that the value of one or several characteristics in the
 * remote GATT server was received
 *
 * It is triggered by several commands: @ref
 * sl_bt_gatt_read_characteristic_value, @ref
 * sl_bt_gatt_read_characteristic_value_from_offset, @ref
 * sl_bt_gatt_read_characteristic_value_by_uuid, @ref
 * sl_bt_gatt_read_multiple_characteristic_values; and when the remote GATT
 * server sends indications or notifications after enabling notifications with
 * @ref sl_bt_gatt_set_characteristic_notification. The parameter @p att_opcode
 * indicates which type of GATT transaction triggered this event. In particular,
 * if the @p att_opcode type is @ref sl_bt_gatt_handle_value_indication (0x1d),
 * the application needs to confirm the indication with @ref
 * sl_bt_gatt_send_characteristic_confirmation.
 */

/** @brief Identifier of the characteristic_value event */
#define sl_bt_evt_gatt_characteristic_value_id                       0x040900a0

/***************************************************************************//**
 * @brief Data structure of the characteristic_value event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_characteristic_value_s
{
  uint8_t    connection;     /**< Connection handle */
  uint16_t   characteristic; /**< GATT characteristic handle. This value is
                                  normally received from the gatt_characteristic
                                  event. */
  uint8_t    att_opcode;     /**< Enum @ref sl_bt_gatt_att_opcode_t. Attribute
                                  opcode, which indicates the GATT transaction
                                  used. */
  uint16_t   offset;         /**< Value offset */
  uint8array value;          /**< Characteristic value */
});

typedef struct sl_bt_evt_gatt_characteristic_value_s sl_bt_evt_gatt_characteristic_value_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_characteristic_value

/**
 * @addtogroup sl_bt_evt_gatt_descriptor_value sl_bt_evt_gatt_descriptor_value
 * @{
 * @brief Indicates that the value of a descriptor in the remote GATT server was
 * received
 *
 * This event is generated by the @ref sl_bt_gatt_read_descriptor_value command.
 */

/** @brief Identifier of the descriptor_value event */
#define sl_bt_evt_gatt_descriptor_value_id                           0x050900a0

/***************************************************************************//**
 * @brief Data structure of the descriptor_value event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_descriptor_value_s
{
  uint8_t    connection; /**< Connection handle */
  uint16_t   descriptor; /**< GATT characteristic descriptor handle */
  uint16_t   offset;     /**< Value offset */
  uint8array value;      /**< Descriptor value */
});

typedef struct sl_bt_evt_gatt_descriptor_value_s sl_bt_evt_gatt_descriptor_value_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_descriptor_value

/**
 * @addtogroup sl_bt_evt_gatt_procedure_completed sl_bt_evt_gatt_procedure_completed
 * @{
 * @brief Indicates that the current GATT procedure was completed successfully
 * or that it failed with an error
 *
 * All GATT commands excluding @ref
 * sl_bt_gatt_write_characteristic_value_without_response and @ref
 * sl_bt_gatt_send_characteristic_confirmation will trigger this event. As a
 * result, the application must wait for this event before issuing another GATT
 * command (excluding the two aforementioned exceptions).
 *
 * <b>Note:</b> After a failed GATT procedure with SL_STATUS_TIMEOUT error,
 * further GATT transactions over this connection are not allowed by the stack.
 */

/** @brief Identifier of the procedure_completed event */
#define sl_bt_evt_gatt_procedure_completed_id                        0x060900a0

/***************************************************************************//**
 * @brief Data structure of the procedure_completed event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_procedure_completed_s
{
  uint8_t  connection; /**< Connection handle */
  uint16_t result;     /**< Result code
                              - <b>0:</b> success
                              - <b>Non-zero:</b> an error has occurred */
});

typedef struct sl_bt_evt_gatt_procedure_completed_s sl_bt_evt_gatt_procedure_completed_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_procedure_completed

/***************************************************************************//**
 *
 * Set the maximum size of ATT Message Transfer Units (MTU). Functionality is
 * the same as @ref sl_bt_gatt_server_set_max_mtu and this setting applies to
 * both GATT client and server. If the given value is too large according to the
 * maximum BGAPI payload size, the system will select the maximum possible value
 * as the maximum ATT_MTU. If the maximum ATT_MTU is larger than 23, the GATT
 * client in the stack will automatically send an MTU exchange request after a
 * Bluetooth connection has been established.
 *
 * @param[in] max_mtu @parblock
 *   Maximum size of Message Transfer Units (MTU) allowed
 *     - Range: 23 to 250
 *
 *   Default: 247
 *   @endparblock
 * @param[out] max_mtu_out The maximum ATT_MTU selected by the system if this
 *   command succeeds
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_set_max_mtu(uint16_t max_mtu, uint16_t *max_mtu_out);

/***************************************************************************//**
 *
 * Discover all primary services of a remote GATT database. This command
 * generates a unique gatt_service event for every discovered primary service.
 * Received @ref sl_bt_evt_gatt_procedure_completed event indicates that this
 * GATT procedure has successfully completed or failed with an error.
 *
 * @param[in] connection Connection handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_service - Discovered service from remote GATT
 *     database
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_discover_primary_services(uint8_t connection);

/***************************************************************************//**
 *
 * Discover primary services with the specified UUID in a remote GATT database.
 * This command generates unique gatt_service event for every discovered primary
 * service. Received @ref sl_bt_evt_gatt_procedure_completed event indicates
 * that this GATT procedure was successfully completed or failed with an error.
 *
 * @param[in] connection Connection handle
 * @param[in] uuid_len Length of data in @p uuid
 * @param[in] uuid Service UUID in little endian format
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_service - Discovered service from remote GATT
 *     database.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_discover_primary_services_by_uuid(uint8_t connection,
                                                         size_t uuid_len,
                                                         const uint8_t* uuid);

/***************************************************************************//**
 *
 * Find the services that are included by a service in a remote GATT database.
 * This command generates a unique gatt_service event for each included service.
 * The received @ref sl_bt_evt_gatt_procedure_completed event indicates that
 * this GATT procedure was successfully completed or failed with an error.
 *
 * @param[in] connection Connection handle
 * @param[in] service GATT service handle. This value is normally received from
 *   the gatt_service event.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_service - Discovered service from remote GATT
 *     database.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_find_included_services(uint8_t connection,
                                              uint32_t service);

/***************************************************************************//**
 *
 * Discover all characteristics of a GATT service from a remote GATT database.
 * This command generates a unique gatt_characteristic event for every
 * discovered characteristic. Received @ref sl_bt_evt_gatt_procedure_completed
 * event indicates that this GATT procedure was successfully completed or failed
 * with an error.
 *
 * @param[in] connection Connection handle
 * @param[in] service GATT service handle. This value is normally received from
 *   the gatt_service event.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_characteristic - Discovered characteristic from
 *     remote GATT database.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_discover_characteristics(uint8_t connection,
                                                uint32_t service);

/***************************************************************************//**
 *
 * Discover all characteristics of a GATT service in a remote GATT database
 * having the specified UUID. This command generates a unique
 * gatt_characteristic event for every discovered characteristic having the
 * specified UUID. Received @ref sl_bt_evt_gatt_procedure_completed event
 * indicates that this GATT procedure was successfully completed or failed with
 * an error.
 *
 * @param[in] connection Connection handle
 * @param[in] service GATT service handle. This value is normally received from
 *   the gatt_service event.
 * @param[in] uuid_len Length of data in @p uuid
 * @param[in] uuid Characteristic UUID in little endian format
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_characteristic - Discovered characteristic from
 *     remote GATT database.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_discover_characteristics_by_uuid(uint8_t connection,
                                                        uint32_t service,
                                                        size_t uuid_len,
                                                        const uint8_t* uuid);

/***************************************************************************//**
 *
 * Discover all descriptors of a GATT characteristic in a remote GATT database.
 * It generates a unique gatt_descriptor event for every discovered descriptor.
 * Received @ref sl_bt_evt_gatt_procedure_completed event indicates that this
 * GATT procedure has successfully completed or failed with an error.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_descriptor - Discovered descriptor from remote GATT
 *     database.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_discover_descriptors(uint8_t connection,
                                            uint16_t characteristic);

/***************************************************************************//**
 *
 * Enable or disable the notifications and indications sent from a remote GATT
 * server. This procedure discovers a characteristic client configuration
 * descriptor and writes the related configuration flags to a remote GATT
 * database. A received @ref sl_bt_evt_gatt_procedure_completed event indicates
 * that this GATT procedure was successfully completed or that it failed with an
 * error.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 * @param[in] flags Enum @ref sl_bt_gatt_client_config_flag_t. Characteristic
 *   client configuration flags. Values:
 *     - <b>sl_bt_gatt_disable (0x0):</b> Disable notifications and indications
 *     - <b>sl_bt_gatt_notification (0x1):</b> Notification
 *     - <b>sl_bt_gatt_indication (0x2):</b> Indication
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *   - @ref sl_bt_evt_gatt_characteristic_value - If an indication or
 *     notification has been enabled for a characteristic, this event is
 *     triggered whenever an indication or notification is sent by the remote
 *     GATT server. The triggering conditions of the GATT server are defined by
 *     an upper level, for example by a profile. <b>As a result, it is possible
 *     that no values are ever received, or that it may take time, depending on
 *     how the server is configured.</b>
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_set_characteristic_notification(uint8_t connection,
                                                       uint16_t characteristic,
                                                       uint8_t flags);

/***************************************************************************//**
 *
 * Send a confirmation to a remote GATT server after receiving a characteristic
 * indication. The @ref sl_bt_evt_gatt_characteristic_value event carries the @p
 * att_opcode containing @ref sl_bt_gatt_handle_value_indication (0x1d), which
 * reveals that an indication has been received and must be confirmed with this
 * command. The confirmation needs to be sent within 30 seconds, otherwise
 * further GATT transactions are not allowed by the remote side.
 *
 * @param[in] connection Connection handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_send_characteristic_confirmation(uint8_t connection);

/***************************************************************************//**
 *
 * Read the value of a characteristic from a remote GATT database. A single @ref
 * sl_bt_evt_gatt_characteristic_value is generated if the characteristic value
 * fits in one ATT PDU. Otherwise, more than one @ref
 * sl_bt_evt_gatt_characteristic_value event is generated because the firmware
 * will automatically use the Read Long Characteristic Values procedure. A
 * received @ref sl_bt_evt_gatt_procedure_completed event indicates that all
 * data was read successfully or that an error response was received.
 *
 * Note that the GATT client does not verify if the requested attribute is a
 * characteristic value. Therefore, before calling this command, ensure that the
 * attribute handle is for a characteristic value, for example, by performing
 * characteristic discovery.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_characteristic_value - Contains the data of a
 *     characteristic sent by the GATT Server.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_read_characteristic_value(uint8_t connection,
                                                 uint16_t characteristic);

/***************************************************************************//**
 *
 * Read a partial characteristic value with a specified offset and maximum
 * length from a remote GATT database. It is equivalent to @ref
 * sl_bt_gatt_read_characteristic_value if both the offset and maximum length
 * parameters are 0. A single @ref sl_bt_evt_gatt_characteristic_value event is
 * generated if the value to read fits in one ATT PDU. Otherwise, more than one
 * @ref sl_bt_evt_gatt_characteristic_value events are generated because the
 * firmware will automatically use the Read Long Characteristic Values
 * procedure. A received @ref sl_bt_evt_gatt_procedure_completed event indicates
 * that all data was read successfully or that an error response was received.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 * @param[in] offset Offset of the characteristic value
 * @param[in] maxlen Maximum bytes to read. If this parameter is 0, all
 *   characteristic values starting at a given offset will be read.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_characteristic_value - Contains data of a
 *     characteristic sent by the GATT Server.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_read_characteristic_value_from_offset(uint8_t connection,
                                                             uint16_t characteristic,
                                                             uint16_t offset,
                                                             uint16_t maxlen);

/***************************************************************************//**
 *
 * Read values of multiple characteristics from a remote GATT database at once.
 * The GATT server returns values in one ATT PDU as the response. If the total
 * set of values is greater than (ATT_MTU - 1) bytes in length, only the first
 * (ATT_MTU - 1) bytes are included. A single @ref
 * sl_bt_evt_gatt_characteristic_value event is generated, in which the
 * characteristic is set to 0 and data in the value parameter is a concatenation
 * of characteristic values in the order they were requested. The received @ref
 * sl_bt_evt_gatt_procedure_completed event indicates either that this GATT
 * procedure was successfully completed or failed with an error.
 *
 * Use this command only for characteristics values that have a known fixed
 * size, except the last one that could have variable length.
 *
 * When the remote GATT server is from Silicon Labs Bluetooth stack, the server
 * returns ATT Invalid PDU (0x04) if this command only reads one characteristic
 * value. The server returns ATT Application Error 0x80 if this command reads
 * the value of a user-type characteristic.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic_list_len Length of data in @p characteristic_list
 * @param[in] characteristic_list List of uint16 characteristic handles each in
 *   little endian format.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_characteristic_value - A concatenation of
 *     characteristic values in the order they were requested
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was either
 *     successfully completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_read_multiple_characteristic_values(uint8_t connection,
                                                           size_t characteristic_list_len,
                                                           const uint8_t* characteristic_list);

/***************************************************************************//**
 *
 * Read characteristic values of a service from a remote GATT database by giving
 * the UUID of the characteristic and the handle of the service containing this
 * characteristic. If multiple characteristic values are received in one ATT
 * PDU, one @ref sl_bt_evt_gatt_characteristic_value event is generated for each
 * value. If the first characteristic value does not fit in one ATT PDU, the
 * firmware automatically uses the Read Long Characteristic Values procedure and
 * generate more @ref sl_bt_evt_gatt_characteristic_value events until the value
 * has been completely read. A received @ref sl_bt_evt_gatt_procedure_completed
 * event indicates that all data was read successfully or that an error response
 * was received.
 *
 * @param[in] connection Connection handle
 * @param[in] service GATT service handle. This value is normally received from
 *   the gatt_service event.
 * @param[in] uuid_len Length of data in @p uuid
 * @param[in] uuid Characteristic UUID in little endian format
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_characteristic_value - Contains the data of a
 *     characteristic sent by the GATT Server.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_read_characteristic_value_by_uuid(uint8_t connection,
                                                         uint32_t service,
                                                         size_t uuid_len,
                                                         const uint8_t* uuid);

/***************************************************************************//**
 *
 * Write the value of a characteristic in a remote GATT database. If the given
 * value does not fit in one ATT PDU, "write long" GATT procedure is used
 * automatically. Received @ref sl_bt_evt_gatt_procedure_completed event
 * indicates that all data was written successfully or that an error response
 * was received.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 * @param[in] value_len Length of data in @p value
 * @param[in] value Characteristic value
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_write_characteristic_value(uint8_t connection,
                                                  uint16_t characteristic,
                                                  size_t value_len,
                                                  const uint8_t* value);

/***************************************************************************//**
 *
 * Write the value of a characteristic in a remote GATT server. It does not
 * generate an event. All failures on the server are ignored silently. For
 * example, if an error is generated in the remote GATT server and the given
 * value is not written into the database, no error message will be reported to
 * the local GATT client. Note that this command can't be used to write long
 * values. At most ATT_MTU - 3 amount of data can be sent once.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 * @param[in] value_len Length of data in @p value
 * @param[in] value Characteristic value
 * @param[out] sent_len The length of data sent to the remote GATT server
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_write_characteristic_value_without_response(uint8_t connection,
                                                                   uint16_t characteristic,
                                                                   size_t value_len,
                                                                   const uint8_t* value,
                                                                   uint16_t *sent_len);

/***************************************************************************//**
 *
 * Add a characteristic value to the write queue of a remote GATT server. It can
 * be used when long attributes need to be written or a set of values needs to
 * be written atomically. At most ATT_MTU - 5 amount of data can be sent at one
 * time. Writes are executed or canceled with the @ref
 * sl_bt_gatt_execute_characteristic_value_write command. Whether the writes
 * succeed or not is indicated in the response of the @ref
 * sl_bt_gatt_execute_characteristic_value_write command.
 *
 * In all use cases where the amount of data to transfer fits into the BGAPI
 * payload, use the command @ref sl_bt_gatt_write_characteristic_value to write
 * long values because it transparently performs the prepare_write and
 * execute_write commands.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 * @param[in] offset Offset of the characteristic value
 * @param[in] value_len Length of data in @p value
 * @param[in] value Value to write into the specified characteristic of the
 *   remote GATT database
 * @param[out] sent_len The length of data sent to the remote GATT server
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_prepare_characteristic_value_write(uint8_t connection,
                                                          uint16_t characteristic,
                                                          uint16_t offset,
                                                          size_t value_len,
                                                          const uint8_t* value,
                                                          uint16_t *sent_len);

/***************************************************************************//**
 *
 * Add a characteristic value to the write queue of a remote GATT server and
 * verifies whether the value was correctly received by the server. Received
 * @ref sl_bt_evt_gatt_procedure_completed event indicates that this GATT
 * procedure was successfully completed or failed with an error. Specifically,
 * error code 0x0194 (data_corrupted) will be returned if the value received
 * from the GATT server's response fails to pass the reliable write
 * verification. At most, ATT_MTU - 5 amount of data can be sent at one time.
 * Writes are executed or canceled with the @ref
 * sl_bt_gatt_execute_characteristic_value_write command. Whether the writes
 * succeed or not is indicated in the response of the @ref
 * sl_bt_gatt_execute_characteristic_value_write command.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 * @param[in] offset Offset of the characteristic value
 * @param[in] value_len Length of data in @p value
 * @param[in] value Value to write into the specified characteristic of the
 *   remote GATT database
 * @param[out] sent_len The length of data sent to the remote GATT server
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_prepare_characteristic_value_reliable_write(uint8_t connection,
                                                                   uint16_t characteristic,
                                                                   uint16_t offset,
                                                                   size_t value_len,
                                                                   const uint8_t* value,
                                                                   uint16_t *sent_len);

/***************************************************************************//**
 *
 * Commit or cancel previously queued writes to a long characteristic of a
 * remote GATT server. Writes are sent to the queue with @ref
 * sl_bt_gatt_prepare_characteristic_value_write command. Content, offset, and
 * length of queued values are validated by this procedure. A received @ref
 * sl_bt_evt_gatt_procedure_completed event indicates that all data was written
 * successfully or that an error response was received.
 *
 * @param[in] connection Connection handle
 * @param[in] flags Enum @ref sl_bt_gatt_execute_write_flag_t. Execute write
 *   flag. Values:
 *     - <b>sl_bt_gatt_cancel (0x0):</b> Cancel all queued writes
 *     - <b>sl_bt_gatt_commit (0x1):</b> Commit all queued writes
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_execute_characteristic_value_write(uint8_t connection,
                                                          uint8_t flags);

/***************************************************************************//**
 *
 * Read the descriptor value of a characteristic in a remote GATT database. A
 * single @ref sl_bt_evt_gatt_descriptor_value event is generated if the
 * descriptor value fits in one ATT PDU. Otherwise, more than one @ref
 * sl_bt_evt_gatt_descriptor_value events are generated because the firmware
 * automatically uses the Read Long Characteristic Values procedure. A received
 * @ref sl_bt_evt_gatt_procedure_completed event indicates that all data was
 * read successfully or that an error response was received.
 *
 * @param[in] connection Connection handle
 * @param[in] descriptor GATT characteristic descriptor handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_descriptor_value - Descriptor value received from the
 *     remote GATT server.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_read_descriptor_value(uint8_t connection,
                                             uint16_t descriptor);

/***************************************************************************//**
 *
 * Write the value of a characteristic descriptor in a remote GATT database. If
 * the given value does not fit in one ATT PDU, "write long" GATT procedure is
 * used automatically. Received @ref sl_bt_evt_gatt_procedure_completed event
 * indicates either that all data was written successfully or that an error
 * response was received.
 *
 * @param[in] connection Connection handle
 * @param[in] descriptor GATT characteristic descriptor handle
 * @param[in] value_len Length of data in @p value
 * @param[in] value Descriptor value
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_write_descriptor_value(uint8_t connection,
                                              uint16_t descriptor,
                                              size_t value_len,
                                              const uint8_t* value);

/** @} */ // end addtogroup sl_bt_gatt

/**
 * @addtogroup sl_bt_gattdb GATT Database
 * @{
 *
 * @brief GATT Database
 *
 * These commands and events are used for managing the local GATT database.
 *
 * Many commands in this class return the handles of created attributes. These
 * handles may change during commit if attributes are not created in the order
 * they present in the database. For example, when creating a new service using
 * the @ref sl_bt_gattdb_add_service command, the service declaration handle
 * returned from this command becomes invalid later when an attribute is added
 * or removed in front of this service. If the user requires that the attribute
 * handles returned from these commands must remain valid after the database
 * update has been committed, attributes must be created in the order they
 * present in the database.
 */

/* Command and Response IDs */
#define sl_bt_cmd_gattdb_new_session_id                              0x00460020
#define sl_bt_cmd_gattdb_add_service_id                              0x01460020
#define sl_bt_cmd_gattdb_remove_service_id                           0x02460020
#define sl_bt_cmd_gattdb_add_included_service_id                     0x03460020
#define sl_bt_cmd_gattdb_remove_included_service_id                  0x04460020
#define sl_bt_cmd_gattdb_add_uuid16_characteristic_id                0x05460020
#define sl_bt_cmd_gattdb_add_uuid128_characteristic_id               0x06460020
#define sl_bt_cmd_gattdb_remove_characteristic_id                    0x07460020
#define sl_bt_cmd_gattdb_add_uuid16_descriptor_id                    0x08460020
#define sl_bt_cmd_gattdb_add_uuid128_descriptor_id                   0x09460020
#define sl_bt_cmd_gattdb_remove_descriptor_id                        0x0a460020
#define sl_bt_cmd_gattdb_start_service_id                            0x0b460020
#define sl_bt_cmd_gattdb_stop_service_id                             0x0c460020
#define sl_bt_cmd_gattdb_start_characteristic_id                     0x0d460020
#define sl_bt_cmd_gattdb_stop_characteristic_id                      0x0e460020
#define sl_bt_cmd_gattdb_commit_id                                   0x0f460020
#define sl_bt_cmd_gattdb_abort_id                                    0x10460020
#define sl_bt_rsp_gattdb_new_session_id                              0x00460020
#define sl_bt_rsp_gattdb_add_service_id                              0x01460020
#define sl_bt_rsp_gattdb_remove_service_id                           0x02460020
#define sl_bt_rsp_gattdb_add_included_service_id                     0x03460020
#define sl_bt_rsp_gattdb_remove_included_service_id                  0x04460020
#define sl_bt_rsp_gattdb_add_uuid16_characteristic_id                0x05460020
#define sl_bt_rsp_gattdb_add_uuid128_characteristic_id               0x06460020
#define sl_bt_rsp_gattdb_remove_characteristic_id                    0x07460020
#define sl_bt_rsp_gattdb_add_uuid16_descriptor_id                    0x08460020
#define sl_bt_rsp_gattdb_add_uuid128_descriptor_id                   0x09460020
#define sl_bt_rsp_gattdb_remove_descriptor_id                        0x0a460020
#define sl_bt_rsp_gattdb_start_service_id                            0x0b460020
#define sl_bt_rsp_gattdb_stop_service_id                             0x0c460020
#define sl_bt_rsp_gattdb_start_characteristic_id                     0x0d460020
#define sl_bt_rsp_gattdb_stop_characteristic_id                      0x0e460020
#define sl_bt_rsp_gattdb_commit_id                                   0x0f460020
#define sl_bt_rsp_gattdb_abort_id                                    0x10460020

/**
 * @brief 
                This enum defines GATT service types.
            
 */
typedef enum
{
  sl_bt_gattdb_primary_service   = 0x0, /**< (0x0) Primary service */
  sl_bt_gattdb_secondary_service = 0x1  /**< (0x1) Secondary service */
} sl_bt_gattdb_service_type_t;

/**
 * @brief 
                This enum defines characteristic and descriptor value types.
            
 */
typedef enum
{
  sl_bt_gattdb_fixed_length_value    = 0x1, /**< (0x1) A fixed-length value
                                                 managed by the local GATT
                                                 server for responding the read
                                                 and write requests of remote
                                                 GATT clients */
  sl_bt_gattdb_variable_length_value = 0x2, /**< (0x2) A variable-length value
                                                 managed by the local GATT
                                                 server for responding the read
                                                 and write requests of remote
                                                 GATT clients */
  sl_bt_gattdb_user_managed_value    = 0x3  /**< (0x3) A value managed by the
                                                 user application for responding
                                                 the read and write requests of
                                                 remote GATT clients. */
} sl_bt_gattdb_value_type_t;

/**
 * @addtogroup sl_bt_gattdb_service_property_flags GATT Service Property Flags
 * @{
 *
 * This enum defines GATT service property flags.
 */

/** The service should be advertised. */
#define SL_BT_GATTDB_ADVERTISED_SERVICE 0x1       

/** @} */ // end GATT Service Property Flags

/**
 * @addtogroup sl_bt_gattdb_security_requirements GATT Attribute Security Requirement Flags
 * @{
 *
 * This enum defines the security requirement flags for GATT characteristic
 * value properties.
 */

/** The read property requires pairing and encrypted connection. */
#define SL_BT_GATTDB_ENCRYPTED_READ       0x1       

/** The read property requires bonding and encrypted connection. */
#define SL_BT_GATTDB_BONDED_READ          0x2       

/** The read property requires authenticated pairing and encrypted connection.
 * */
#define SL_BT_GATTDB_AUTHENTICATED_READ   0x4       

/** The write property requires pairing and encrypted connection. */
#define SL_BT_GATTDB_ENCRYPTED_WRITE      0x8       

/** The write property requires bonding and encrypted connection. */
#define SL_BT_GATTDB_BONDED_WRITE         0x10      

/** The write property requires authenticated pairing and encrypted connection.
 * */
#define SL_BT_GATTDB_AUTHENTICATED_WRITE  0x20      

/** The notification and indication properties require pairing and encrypted
 * connection. */
#define SL_BT_GATTDB_ENCRYPTED_NOTIFY     0x40      

/** The notification and indication properties require bonding and encrypted
 * connection. */
#define SL_BT_GATTDB_BONDED_NOTIFY        0x80      

/** The notification and indication properties require authenticated pairing and
 * encrypted connection. */
#define SL_BT_GATTDB_AUTHENTICATED_NOTIFY 0x100     

/** @} */ // end GATT Attribute Security Requirement Flags

/**
 * @addtogroup sl_bt_gattdb_flags GATT Database Flags
 * @{
 *
 * This enum defines the options of GATT attribute management.
 */

/** Do not automatically create a Client Characteristic Configuration descriptor
 * when adding a characteristic that has the notify or indicate property. */
#define SL_BT_GATTDB_NO_AUTO_CCCD 0x1       

/** @} */ // end GATT Database Flags

/**
 * @addtogroup sl_bt_gattdb_characteristic_properties GATT Characteristic Property Flags
 * @{
 *
 * This enum defines the property flags for GATT characteristic values. Lower
 * byte is Characteristic Properties and higher byte is Characteristic Extended
 * Properties.
 */

/** A GATT client can read the characteristic value. */
#define SL_BT_GATTDB_CHARACTERISTIC_READ              0x2       

/** A GATT client can write the characteristic value without a response. */
#define SL_BT_GATTDB_CHARACTERISTIC_WRITE_NO_RESPONSE 0x4       

/** A GATT client can write the characteristic value. */
#define SL_BT_GATTDB_CHARACTERISTIC_WRITE             0x8       

/** The characteristic value can be notified without acknowledgment. */
#define SL_BT_GATTDB_CHARACTERISTIC_NOTIFY            0x10      

/** The characteristic value can be notified with acknowledgment. */
#define SL_BT_GATTDB_CHARACTERISTIC_INDICATE          0x20      

/** The additional characteristic properties are defined. */
#define SL_BT_GATTDB_CHARACTERISTIC_EXTENDED_PROPS    0x80      

/** The characteristic value supports reliable write. */
#define SL_BT_GATTDB_CHARACTERISTIC_RELIABLE_WRITE    0x101     

/** @} */ // end GATT Characteristic Property Flags

/**
 * @addtogroup sl_bt_gattdb_descriptor_properties GATT Descriptor Property Flags
 * @{
 *
 * This enum defines the property flags for GATT characteristic descriptors.
 */

/** A GATT client can read the descriptor value. */
#define SL_BT_GATTDB_DESCRIPTOR_READ       0x1       

/** A GATT client can write the descriptor value. */
#define SL_BT_GATTDB_DESCRIPTOR_WRITE      0x2       

/** The descriptor is local only and should be invisible to GATT clients. */
#define SL_BT_GATTDB_DESCRIPTOR_LOCAL_ONLY 0x200     

/** @} */ // end GATT Descriptor Property Flags

/***************************************************************************//**
 *
 * Start a new GATT database update session. If the operation is successful, the
 * Bluetooth stack returns a session ID, with which the GATT database can be
 * updated by calling other database management APIs of this class. Changes in
 * the database are not immediately saved. Unsaved changes are invisible to a
 * connected remote GATT client.
 *
 * After all changes were performed successfully, commit the changes using the
 * @ref sl_bt_gattdb_commit command. The Bluetooth stack will save the changes
 * and handle GATT caching as needed. Unsaved database changes can also be
 * cancelled by calling the @ref sl_bt_gattdb_abort command. In either case,
 * after a commit or abort command is called, the current session is closed and
 * the session ID becomes invalid.
 *
 * Only one session is allowed at a time. Error SL_STATUS_ALREADY_EXISTS is
 * returned if another session has been started already.
 *
 * @param[out] session The database update session ID
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_new_session(uint16_t *session);

/***************************************************************************//**
 *
 * Add a service into the local GATT database. When successful, the service is
 * appended to the service list and is in stopped state. Use @ref
 * sl_bt_gattdb_start_service command to set it visible to remote GATT clients.
 *
 * You are not allowed to add the Generic Attribute Profile service. If the
 * application needs GATT caching, enable the feature in the configuration of
 * this component and the GATT server will handle GATT caching according to the
 * procedures specified by the Bluetooth core specification.
 *
 * @param[in] session The database update session ID
 * @param[in] type Enum @ref sl_bt_gattdb_service_type_t. The service type.
 *   Values:
 *     - <b>sl_bt_gattdb_primary_service (0x0):</b> Primary service
 *     - <b>sl_bt_gattdb_secondary_service (0x1):</b> Secondary service
 * @param[in] property Service properties. Value: 0 or bit flag @ref
 *   SL_BT_GATTDB_ADVERTISED_SERVICE
 * @param[in] uuid_len Length of data in @p uuid
 * @param[in] uuid The service UUID in little endian format
 * @param[out] service The service declaration attribute handle. This handle is
 *   ensured valid in current session. It may change after the session if
 *   attributes have been inserted or deleted in front of it.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_add_service(uint16_t session,
                                     uint8_t type,
                                     uint8_t property,
                                     size_t uuid_len,
                                     const uint8_t* uuid,
                                     uint16_t *service);

/***************************************************************************//**
 *
 * Remove a service and its characteristics from the local GATT database.
 *
 * @param[in] session The database update session ID
 * @param[in] service The service declaration attribute handle of the service
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_remove_service(uint16_t session, uint16_t service);

/***************************************************************************//**
 *
 * Add an included-service attribute to a service.
 *
 * @param[in] session The database update session ID
 * @param[in] service The service declaration attribute handle of the service
 *   which the included-service attribute is added to
 * @param[in] included_service The service declaration attribute handle of the
 *   service to be included
 * @param[out] attribute The included-service attribute handle. This handle is
 *   ensured valid in current session. It may change after the session if
 *   attributes have been inserted or deleted in front of it.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_add_included_service(uint16_t session,
                                              uint16_t service,
                                              uint16_t included_service,
                                              uint16_t *attribute);

/***************************************************************************//**
 *
 * Remove an included-service attribute from a service.
 *
 * @param[in] session The database update session ID
 * @param[in] attribute The included-service attribute handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_remove_included_service(uint16_t session,
                                                 uint16_t attribute);

/***************************************************************************//**
 *
 * Add a 16-bits UUID characteristic to a service. On success, the
 * characteristic is appended to the characteristic list of the service and it
 * inherits the started or stopped state of the service. In addition, it can be
 * started and stopped separately with the @ref
 * sl_bt_gattdb_start_characteristic and @ref sl_bt_gattdb_stop_characteristic
 * commands.
 *
 * If the @p flag parameter does not set @ref SL_BT_GATTDB_NO_AUTO_CCCD, the
 * stack will automatically add a Client Characteristic Configuration descriptor
 * to this characteristic when it has the notify or indicate property. If @ref
 * SL_BT_GATTDB_NO_AUTO_CCCD is set, the user application should add the
 * descriptor separately as needed.
 *
 * A Characteristic Extended Properties descriptor is automatically added if the
 * reliable write property is set.
 *
 * Use the @ref sl_bt_gattdb_add_uuid128_characteristic command to add a
 * 128-bits UUID characteristic.
 *
 * @param[in] session The database update session ID
 * @param[in] service The service declaration attribute handle of the service
 *   which the characteristic is added to
 * @param[in] property Characteristic value properties. Value: bitmask of @ref
 *   sl_bt_gattdb_characteristic_properties
 * @param[in] security Security requirement. Value: 0 or bitmask of @ref
 *   sl_bt_gattdb_security_requirements. A security requirement flag for a
 *   property is ignored if the property is not set for the characteristic
 *   value.
 * @param[in] flag Option flags. Value: 0 or bitmask of @ref sl_bt_gattdb_flags.
 * @param[in] uuid The 16-bits UUID in little endian format
 * @param[in] value_type Enum @ref sl_bt_gattdb_value_type_t. The value type.
 *   Values:
 *     - <b>sl_bt_gattdb_fixed_length_value (0x1):</b> A fixed-length value
 *       managed by the local GATT server for responding the read and write
 *       requests of remote GATT clients
 *     - <b>sl_bt_gattdb_variable_length_value (0x2):</b> A variable-length
 *       value managed by the local GATT server for responding the read and
 *       write requests of remote GATT clients
 *     - <b>sl_bt_gattdb_user_managed_value (0x3):</b> A value managed by the
 *       user application for responding the read and write requests of remote
 *       GATT clients.
 * @param[in] maxlen The maximum length of the characteristic value. Ignored if
 *   @p value_type is @ref sl_bt_gattdb_user_managed_value.
 * @param[in] value_len Length of data in @p value
 * @param[in] value The initial characteristic value. Length of this value must
 *   be less than or equal to @p maxlen. Ignored if @p value_type is @ref
 *   sl_bt_gattdb_user_managed_value.
 * @param[out] characteristic The characteristic value attribute handle. This
 *   handle is ensured valid in current session. It may change after the session
 *   if attributes have been inserted or deleted in front of it.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_add_uuid16_characteristic(uint16_t session,
                                                   uint16_t service,
                                                   uint16_t property,
                                                   uint16_t security,
                                                   uint8_t flag,
                                                   sl_bt_uuid_16_t uuid,
                                                   uint8_t value_type,
                                                   uint16_t maxlen,
                                                   size_t value_len,
                                                   const uint8_t* value,
                                                   uint16_t *characteristic);

/***************************************************************************//**
 *
 * Add a 128-bits UUID characteristic to a service. When successful, the
 * characteristic is appended to the characteristic list of the service and
 * inherits the started or stopped state of the service. Additionally, it can be
 * started and stopped separately with the @ref
 * sl_bt_gattdb_start_characteristic and @ref sl_bt_gattdb_stop_characteristic
 * commands.
 *
 * If the @p flag parameter does not set @ref SL_BT_GATTDB_NO_AUTO_CCCD, the
 * stack will automatically add a Client Characteristic Configuration descriptor
 * to this characteristic when it has the notify or indicate property. If @ref
 * SL_BT_GATTDB_NO_AUTO_CCCD is set, the user application should add the
 * descriptor separately as needed.
 *
 * A Characteristic Extended Properties descriptor is automatically added if the
 * reliable write property is set.
 *
 * Use the @ref sl_bt_gattdb_add_uuid16_characteristic command to add a 16-bits
 * UUID characteristic.
 *
 * @param[in] session The database update session ID
 * @param[in] service The service declaration attribute handle of the service
 *   which the characteristic is added to
 * @param[in] property Characteristic value properties. Value: bitmask of @ref
 *   sl_bt_gattdb_characteristic_properties
 * @param[in] security Security requirement. Value: 0 or bitmask of @ref
 *   sl_bt_gattdb_security_requirements. A security requirement flag for a
 *   property is ignored if the property is not set for the characteristic
 *   value.
 * @param[in] flag Option flags. Value: 0 or bitmask of @ref sl_bt_gattdb_flags.
 * @param[in] uuid The 128-bits UUID in little endian format
 * @param[in] value_type Enum @ref sl_bt_gattdb_value_type_t. The value type.
 *   Values:
 *     - <b>sl_bt_gattdb_fixed_length_value (0x1):</b> A fixed-length value
 *       managed by the local GATT server for responding the read and write
 *       requests of remote GATT clients
 *     - <b>sl_bt_gattdb_variable_length_value (0x2):</b> A variable-length
 *       value managed by the local GATT server for responding the read and
 *       write requests of remote GATT clients
 *     - <b>sl_bt_gattdb_user_managed_value (0x3):</b> A value managed by the
 *       user application for responding the read and write requests of remote
 *       GATT clients.
 * @param[in] maxlen The maximum length of the characteristic value. Ignored if
 *   @p value_type is @ref sl_bt_gattdb_user_managed_value.
 * @param[in] value_len Length of data in @p value
 * @param[in] value The initial characteristic value. Length of this value must
 *   be less than or equal to @p maxlen. Ignored if @p value_type is @ref
 *   sl_bt_gattdb_user_managed_value.
 * @param[out] characteristic The characteristic value attribute handle. This
 *   handle is ensured valid in current session. It may change after the session
 *   if attributes have been inserted or deleted in front of it.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_add_uuid128_characteristic(uint16_t session,
                                                    uint16_t service,
                                                    uint16_t property,
                                                    uint16_t security,
                                                    uint8_t flag,
                                                    uuid_128 uuid,
                                                    uint8_t value_type,
                                                    uint16_t maxlen,
                                                    size_t value_len,
                                                    const uint8_t* value,
                                                    uint16_t *characteristic);

/***************************************************************************//**
 *
 * Remove a characteristic and its descriptors from a service.
 *
 * @param[in] session The database update session ID
 * @param[in] characteristic The characteristic value attribute handle of the
 *   characteristic
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_remove_characteristic(uint16_t session,
                                               uint16_t characteristic);

/***************************************************************************//**
 *
 * Add a 16-bits UUID descriptor to a characteristic. When successful, the
 * descriptor is appended to the descriptor list of the characteristic and
 * inherits the started or stopped state of the characteristic.
 *
 * This command does not support adding Characteristic Extended Properties
 * descriptors. This descriptor is automatically added if the characteristic
 * value has the reliable-write property or when a Characteristic User
 * Description descriptor is added and the user description has the write
 * property.
 *
 * Use the @ref sl_bt_gattdb_add_uuid128_descriptor command to add a 128-bits
 * UUID descriptor.
 *
 * @param[in] session The database update session ID
 * @param[in] characteristic The characteristic value attribute handle of the
 *   characteristic the descriptor is added to
 * @param[in] property The descriptor properties. Value: bitmask of @ref
 *   sl_bt_gattdb_descriptor_properties
 * @param[in] security Security requirement. Value: 0 or bitmask of @ref
 *   sl_bt_gattdb_security_requirements. A security requirement flag for a
 *   property is ignored if the property is not set for the descriptor.
 * @param[in] uuid The 16-bits UUID in little endian format
 * @param[in] value_type Enum @ref sl_bt_gattdb_value_type_t. The value type.
 *   Ignored if this is a Client Characteristic Configuration descriptor.
 *   Values:
 *     - <b>sl_bt_gattdb_fixed_length_value (0x1):</b> A fixed-length value
 *       managed by the local GATT server for responding the read and write
 *       requests of remote GATT clients
 *     - <b>sl_bt_gattdb_variable_length_value (0x2):</b> A variable-length
 *       value managed by the local GATT server for responding the read and
 *       write requests of remote GATT clients
 *     - <b>sl_bt_gattdb_user_managed_value (0x3):</b> A value managed by the
 *       user application for responding the read and write requests of remote
 *       GATT clients.
 * @param[in] maxlen The maximum length of the descriptor value. Ignored if @p
 *   value_type is sl_bt_gattdb_user_managed_value, or if this is a Client
 *   Characteristic Configuration descriptor.
 * @param[in] value_len Length of data in @p value
 * @param[in] value The initial descriptor value. Length of this value must be
 *   less than or equal to @p maxlen. Ingored if value type is @ref
 *   sl_bt_gattdb_user_managed_value, or if this is a Client Characteristic
 *   Configuration descriptor.
 * @param[out] descriptor The descriptor attribute handle. This handle is
 *   ensured valid in current session. It may change after the session if
 *   attributes have been inserted or deleted in front of it.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_add_uuid16_descriptor(uint16_t session,
                                               uint16_t characteristic,
                                               uint16_t property,
                                               uint16_t security,
                                               sl_bt_uuid_16_t uuid,
                                               uint8_t value_type,
                                               uint16_t maxlen,
                                               size_t value_len,
                                               const uint8_t* value,
                                               uint16_t *descriptor);

/***************************************************************************//**
 *
 * Add a 128-bits UUID descriptor to a characteristic. When successful, the
 * descriptor is appended to the descriptor list of the characteristic and
 * inherits the started or stopped state of the characteristic.
 *
 * This command does not support adding Characteristic Extended Properties
 * descriptors. This descriptor is automatically added if the characteristic
 * value has the reliable-write property or when a Characteristic User
 * Description descriptor is added and the user description has the write
 * property.
 *
 * Use the @ref sl_bt_gattdb_add_uuid16_descriptor command to add a 16-bits UUID
 * descriptor.
 *
 * @param[in] session The database update session ID
 * @param[in] characteristic The characteristic value attribute handle of the
 *   characteristic the descriptor is added to
 * @param[in] property Bitmask of characteristic descriptor properties
 * @param[in] security Security requirement. Value: 0 or bitmask of @ref
 *   sl_bt_gattdb_security_requirements. A security requirement flag for a
 *   property is ignored if the property is not set for the descriptor.
 * @param[in] uuid The 128-bits UUID in little endian format
 * @param[in] value_type Enum @ref sl_bt_gattdb_value_type_t. The value type.
 *   Ignored if this is a Client Characteristic Configuration descriptor.
 *   Values:
 *     - <b>sl_bt_gattdb_fixed_length_value (0x1):</b> A fixed-length value
 *       managed by the local GATT server for responding the read and write
 *       requests of remote GATT clients
 *     - <b>sl_bt_gattdb_variable_length_value (0x2):</b> A variable-length
 *       value managed by the local GATT server for responding the read and
 *       write requests of remote GATT clients
 *     - <b>sl_bt_gattdb_user_managed_value (0x3):</b> A value managed by the
 *       user application for responding the read and write requests of remote
 *       GATT clients.
 * @param[in] maxlen The maximum length of the descriptor value. Ignored if @p
 *   value_type is sl_bt_gattdb_user_managed_value, or if this is a Client
 *   Characteristic Configuration descriptor.
 * @param[in] value_len Length of data in @p value
 * @param[in] value The initial descriptor value. Length of this value must be
 *   less than or equal to @p maxlen. Ingored if value type is @ref
 *   sl_bt_gattdb_user_managed_value, or if this is a Client Characteristic
 *   Configuration descriptor.
 * @param[out] descriptor The descriptor attribute handle. This handle is
 *   ensured valid in current session. It may change after the session if
 *   attributes have been inserted or deleted in front of it.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_add_uuid128_descriptor(uint16_t session,
                                                uint16_t characteristic,
                                                uint16_t property,
                                                uint16_t security,
                                                uuid_128 uuid,
                                                uint8_t value_type,
                                                uint16_t maxlen,
                                                size_t value_len,
                                                const uint8_t* value,
                                                uint16_t *descriptor);

/***************************************************************************//**
 *
 * Remove a descriptor from a characteristic.
 *
 * @param[in] session The database update session ID
 * @param[in] descriptor The descriptor handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_remove_descriptor(uint16_t session,
                                           uint16_t descriptor);

/***************************************************************************//**
 *
 * Start a service, so that the service and its attributes including
 * characteristics and descriptors become visible to remote GATT clients after
 * this change has been committed.
 *
 * @param[in] session The database update session ID
 * @param[in] service The service declaration attribute handle of the service
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_start_service(uint16_t session, uint16_t service);

/***************************************************************************//**
 *
 * Stop a service, so that the service and its attributes including
 * characteristics and descriptors become invisible to remote GATT clients after
 * this change has been committed.
 *
 * @param[in] session The database update session ID
 * @param[in] service The service declaration attribute handle of the service
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_stop_service(uint16_t session, uint16_t service);

/***************************************************************************//**
 *
 * Start a characteristic, so that the characteristic and its attributes become
 * visible to remote GATT clients after this change has been committed.
 * SL_STATUS_INVALID_STATE error is returned if the parent service is not
 * started.
 *
 * @param[in] session The database update session ID
 * @param[in] characteristic The characteristic value attribute handle of the
 *   characteristic
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_start_characteristic(uint16_t session,
                                              uint16_t characteristic);

/***************************************************************************//**
 *
 * Stop a characteristic, so that the characteristic and its attributes become
 * invisible to remote GATT clients after this change has been committed.
 *
 * @param[in] session The database update session ID
 * @param[in] characteristic The characteristic value attribute handle of the
 *   characteristic
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_stop_characteristic(uint16_t session,
                                             uint16_t characteristic);

/***************************************************************************//**
 *
 * Save all changes performed in current session and close the session. The
 * stack will assign final handles to new and affected attributes, and handle
 * GATT caching as needed. The stack removes the client characteristic
 * configurations of non-connected GATT clients except the service-changed
 * configuration. For connected GATT clients during this database change, the
 * stack removes the configurations to the removed characteristics. The session
 * ID, temporary attribute handles returned during this session, and other
 * existing attribute handles that are after newly added or removed attributes
 * are invalidated.
 *
 * Some attribute handles returned in this session may become invalid if
 * attributes are not created in the order they present in the database. In this
 * case, attribute handle cache of the database in the user application must be
 * refreshed to avoid accidentally using an invalidated handle in subsequent
 * operations.
 *
 * @param[in] session The database update session ID
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_commit(uint16_t session);

/***************************************************************************//**
 *
 * Cancel all changes performed in the current session and close the session.
 * The database remains in the same state it was in just before the session was
 * started. The session ID and all temporary attribute handles returned during
 * this session are invalidated.
 *
 * @param[in] session The database update session ID
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gattdb_abort(uint16_t session);

/** @} */ // end addtogroup sl_bt_gattdb

/**
 * @addtogroup sl_bt_gatt_server GATT Server
 * @{
 *
 * @brief GATT Server
 *
 * These commands and events are used for accessing to the local GATT server and
 * database.
 */

/* Command and Response IDs */
#define sl_bt_cmd_gatt_server_set_max_mtu_id                         0x0a0a0020
#define sl_bt_cmd_gatt_server_get_mtu_id                             0x0b0a0020
#define sl_bt_cmd_gatt_server_find_attribute_id                      0x060a0020
#define sl_bt_cmd_gatt_server_read_attribute_value_id                0x000a0020
#define sl_bt_cmd_gatt_server_read_attribute_type_id                 0x010a0020
#define sl_bt_cmd_gatt_server_write_attribute_value_id               0x020a0020
#define sl_bt_cmd_gatt_server_send_user_read_response_id             0x030a0020
#define sl_bt_cmd_gatt_server_send_user_write_response_id            0x040a0020
#define sl_bt_cmd_gatt_server_send_characteristic_notification_id    0x050a0020
#define sl_bt_cmd_gatt_server_send_notification_id                   0x0f0a0020
#define sl_bt_cmd_gatt_server_send_indication_id                     0x100a0020
#define sl_bt_cmd_gatt_server_notify_all_id                          0x110a0020
#define sl_bt_cmd_gatt_server_read_client_configuration_id           0x120a0020
#define sl_bt_cmd_gatt_server_send_user_prepare_write_response_id    0x140a0020
#define sl_bt_cmd_gatt_server_set_capabilities_id                    0x080a0020
#define sl_bt_cmd_gatt_server_enable_capabilities_id                 0x0c0a0020
#define sl_bt_cmd_gatt_server_disable_capabilities_id                0x0d0a0020
#define sl_bt_cmd_gatt_server_get_enabled_capabilities_id            0x0e0a0020
#define sl_bt_cmd_gatt_server_read_client_supported_features_id      0x150a0020
#define sl_bt_rsp_gatt_server_set_max_mtu_id                         0x0a0a0020
#define sl_bt_rsp_gatt_server_get_mtu_id                             0x0b0a0020
#define sl_bt_rsp_gatt_server_find_attribute_id                      0x060a0020
#define sl_bt_rsp_gatt_server_read_attribute_value_id                0x000a0020
#define sl_bt_rsp_gatt_server_read_attribute_type_id                 0x010a0020
#define sl_bt_rsp_gatt_server_write_attribute_value_id               0x020a0020
#define sl_bt_rsp_gatt_server_send_user_read_response_id             0x030a0020
#define sl_bt_rsp_gatt_server_send_user_write_response_id            0x040a0020
#define sl_bt_rsp_gatt_server_send_characteristic_notification_id    0x050a0020
#define sl_bt_rsp_gatt_server_send_notification_id                   0x0f0a0020
#define sl_bt_rsp_gatt_server_send_indication_id                     0x100a0020
#define sl_bt_rsp_gatt_server_notify_all_id                          0x110a0020
#define sl_bt_rsp_gatt_server_read_client_configuration_id           0x120a0020
#define sl_bt_rsp_gatt_server_send_user_prepare_write_response_id    0x140a0020
#define sl_bt_rsp_gatt_server_set_capabilities_id                    0x080a0020
#define sl_bt_rsp_gatt_server_enable_capabilities_id                 0x0c0a0020
#define sl_bt_rsp_gatt_server_disable_capabilities_id                0x0d0a0020
#define sl_bt_rsp_gatt_server_get_enabled_capabilities_id            0x0e0a0020
#define sl_bt_rsp_gatt_server_read_client_supported_features_id      0x150a0020

/**
 * @brief 
                These values define whether the server is to sent notifications or indications to a remote GATT
                server.
            
 */
typedef enum
{
  sl_bt_gatt_server_disable                     = 0x0, /**< (0x0) Disable
                                                            notifications and
                                                            indications. */
  sl_bt_gatt_server_notification                = 0x1, /**< (0x1) The
                                                            characteristic value
                                                            shall be notified. */
  sl_bt_gatt_server_indication                  = 0x2, /**< (0x2) The
                                                            characteristic value
                                                            shall be indicated. */
  sl_bt_gatt_server_notification_and_indication = 0x3  /**< (0x3) The
                                                            characteristic value
                                                            notification and
                                                            indication are
                                                            enabled, application
                                                            decides which one to
                                                            send. */
} sl_bt_gatt_server_client_configuration_t;

/**
 * @brief 
                These values describe whether the characteristic client configuration was changed or whether a
                characteristic confirmation was received.
            
 */
typedef enum
{
  sl_bt_gatt_server_client_config = 0x1, /**< (0x1) Characteristic client
                                              configuration has been changed. */
  sl_bt_gatt_server_confirmation  = 0x2  /**< (0x2) Characteristic confirmation
                                              has been received. */
} sl_bt_gatt_server_characteristic_status_flag_t;

/**
 * @addtogroup sl_bt_evt_gatt_server_attribute_value sl_bt_evt_gatt_server_attribute_value
 * @{
 * @brief Indicates that the value of an attribute in the local GATT database
 * was changed by a remote GATT client
 *
 * The parameter @p att_opcode describes which GATT procedure was used to change
 * the value.
 */

/** @brief Identifier of the attribute_value event */
#define sl_bt_evt_gatt_server_attribute_value_id                     0x000a00a0

/***************************************************************************//**
 * @brief Data structure of the attribute_value event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_server_attribute_value_s
{
  uint8_t    connection; /**< Connection handle */
  uint16_t   attribute;  /**< Attribute Handle */
  uint8_t    att_opcode; /**< Enum @ref sl_bt_gatt_att_opcode_t. Attribute
                              opcode that informs the procedure from which the
                              value was received. */
  uint16_t   offset;     /**< Value offset */
  uint8array value;      /**< Value */
});

typedef struct sl_bt_evt_gatt_server_attribute_value_s sl_bt_evt_gatt_server_attribute_value_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_server_attribute_value

/**
 * @addtogroup sl_bt_evt_gatt_server_user_read_request sl_bt_evt_gatt_server_user_read_request
 * @{
 * @brief Indicates that a remote GATT client is attempting to read a value of
 * an attribute from the local GATT database, where the attribute was defined in
 * the GATT database XML file to have the type="user"
 *
 * The parameter @p att_opcode informs which GATT procedure was used to read the
 * value. The application needs to respond to this request by using the @ref
 * sl_bt_gatt_server_send_user_read_response command within 30 seconds,
 * otherwise further GATT transactions are not allowed by the remote side.
 */

/** @brief Identifier of the user_read_request event */
#define sl_bt_evt_gatt_server_user_read_request_id                   0x010a00a0

/***************************************************************************//**
 * @brief Data structure of the user_read_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_server_user_read_request_s
{
  uint8_t  connection;     /**< Connection handle */
  uint16_t characteristic; /**< GATT characteristic handle. This value is
                                normally received from the gatt_characteristic
                                event. */
  uint8_t  att_opcode;     /**< Enum @ref sl_bt_gatt_att_opcode_t. Attribute
                                opcode that informs the procedure from which the
                                value was received. */
  uint16_t offset;         /**< Value offset */
});

typedef struct sl_bt_evt_gatt_server_user_read_request_s sl_bt_evt_gatt_server_user_read_request_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_server_user_read_request

/**
 * @addtogroup sl_bt_evt_gatt_server_user_write_request sl_bt_evt_gatt_server_user_write_request
 * @{
 * @brief Indicates that a remote GATT client is attempting to write a value of
 * an attribute into the local GATT database, where the attribute was defined in
 * the GATT database XML file to have the type="user"
 *
 * The parameter @p att_opcode informs which attribute procedure was used to
 * write the value. If the @p att_opcode is @ref sl_bt_gatt_write_request (see
 * @ref sl_bt_gatt_att_opcode_t), the application needs to respond to this
 * request by using the @ref sl_bt_gatt_server_send_user_write_response command
 * within 30 seconds, otherwise further GATT transactions are not allowed by the
 * remote side. If the @p att_opcode is @ref sl_bt_gatt_prepare_write_request,
 * the application needs to respond to this request by using the @ref
 * sl_bt_gatt_server_send_user_prepare_write_response command within 30 seconds,
 * otherwise further GATT transactions are not allowed by the remote side. If
 * the value of @p att_opcode is @ref sl_bt_gatt_execute_write_request, it
 * indicates that there was one or more prepare writes earlier and now the GATT
 * server is processing the execute write, the value of @p characteristic is set
 * to 0 and should be ignored. The event @ref
 * sl_bt_evt_gatt_server_execute_write_completed will be emitted after
 * responding to @ref sl_bt_gatt_execute_write_request by using @ref
 * sl_bt_gatt_server_send_user_write_response.
 */

/** @brief Identifier of the user_write_request event */
#define sl_bt_evt_gatt_server_user_write_request_id                  0x020a00a0

/***************************************************************************//**
 * @brief Data structure of the user_write_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_server_user_write_request_s
{
  uint8_t    connection;     /**< Connection handle */
  uint16_t   characteristic; /**< GATT characteristic handle. This value is
                                  normally received from the gatt_characteristic
                                  event. */
  uint8_t    att_opcode;     /**< Enum @ref sl_bt_gatt_att_opcode_t. Attribute
                                  opcode that informs the procedure from which
                                  the value was received. */
  uint16_t   offset;         /**< Value offset */
  uint8array value;          /**< Value */
});

typedef struct sl_bt_evt_gatt_server_user_write_request_s sl_bt_evt_gatt_server_user_write_request_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_server_user_write_request

/**
 * @addtogroup sl_bt_evt_gatt_server_characteristic_status sl_bt_evt_gatt_server_characteristic_status
 * @{
 * @brief Indicates either that a local Client Characteristic Configuration
 * descriptor was changed by the remote GATT client, or that a confirmation from
 * the remote GATT client was received upon a successful reception of the
 * indication
 *
 * A confirmation by the remote GATT client should be received within 30 seconds
 * after an indication was sent with the @ref sl_bt_gatt_server_send_indication
 * command, otherwise further GATT transactions over this connection are not
 * allowed by the stack.
 */

/** @brief Identifier of the characteristic_status event */
#define sl_bt_evt_gatt_server_characteristic_status_id               0x030a00a0

/***************************************************************************//**
 * @brief Data structure of the characteristic_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_server_characteristic_status_s
{
  uint8_t  connection;          /**< Connection handle */
  uint16_t characteristic;      /**< GATT characteristic handle. This value is
                                     normally received from the
                                     gatt_characteristic event. */
  uint8_t  status_flags;        /**< Enum @ref
                                     sl_bt_gatt_server_characteristic_status_flag_t.
                                     Describes whether Client Characteristic
                                     Configuration was changed or if a
                                     confirmation was received. Values:
                                       - <b>sl_bt_gatt_server_client_config
                                         (0x1):</b> Characteristic client
                                         configuration has been changed.
                                       - <b>sl_bt_gatt_server_confirmation
                                         (0x2):</b> Characteristic confirmation
                                         has been received. */
  uint16_t client_config_flags; /**< Enum @ref
                                     sl_bt_gatt_server_client_configuration_t.
                                     This field carries the new value of the
                                     Client Characteristic Configuration. If the
                                     status_flags is 0x2 (confirmation
                                     received), the value of this field can be
                                     ignored. */
  uint16_t client_config;       /**< The handle of client-config descriptor. */
});

typedef struct sl_bt_evt_gatt_server_characteristic_status_s sl_bt_evt_gatt_server_characteristic_status_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_server_characteristic_status

/**
 * @addtogroup sl_bt_evt_gatt_server_execute_write_completed sl_bt_evt_gatt_server_execute_write_completed
 * @{
 * @brief Execute write completed event indicates that the execute write command
 * from a remote GATT client has completed with the given result
 */

/** @brief Identifier of the execute_write_completed event */
#define sl_bt_evt_gatt_server_execute_write_completed_id             0x040a00a0

/***************************************************************************//**
 * @brief Data structure of the execute_write_completed event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_server_execute_write_completed_s
{
  uint8_t  connection; /**< Connection handle */
  uint16_t result;     /**< Execute write result */
});

typedef struct sl_bt_evt_gatt_server_execute_write_completed_s sl_bt_evt_gatt_server_execute_write_completed_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_server_execute_write_completed

/**
 * @addtogroup sl_bt_evt_gatt_server_indication_timeout sl_bt_evt_gatt_server_indication_timeout
 * @{
 * @brief This event indicates confirmation from the remote GATT client has not
 * been received within 30 seconds after an indication was sent
 *
 * Furthermore, the stack does not allow GATT transactions over this connection.
 */

/** @brief Identifier of the indication_timeout event */
#define sl_bt_evt_gatt_server_indication_timeout_id                  0x050a00a0

/***************************************************************************//**
 * @brief Data structure of the indication_timeout event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_server_indication_timeout_s
{
  uint8_t connection; /**< Connection handle */
});

typedef struct sl_bt_evt_gatt_server_indication_timeout_s sl_bt_evt_gatt_server_indication_timeout_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_server_indication_timeout

/***************************************************************************//**
 *
 * Set the maximum size of ATT Message Transfer Units (MTU). The functionality
 * is the same as @ref sl_bt_gatt_set_max_mtu and this setting applies to both
 * GATT client and server. If the given value is too large according to the
 * maximum BGAPI payload size, the system will select the maximum possible value
 * as the maximum ATT_MTU. If the maximum ATT_MTU is larger than 23, the GATT
 * client in the stack will automatically send an MTU exchange request after a
 * Bluetooth connection was established.
 *
 * @param[in] max_mtu @parblock
 *   Maximum size of Message Transfer Units (MTU) allowed
 *     - Range: 23 to 250
 *
 *   Default: 247
 *   @endparblock
 * @param[out] max_mtu_out The maximum ATT_MTU selected by the system if this
 *   command succeeded
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_set_max_mtu(uint16_t max_mtu,
                                          uint16_t *max_mtu_out);

/***************************************************************************//**
 *
 * Get the size of ATT Message Transfer Units (MTU) for a connection.
 *
 * @param[in] connection Connection handle
 * @param[out] mtu The maximum ATT_MTU used by the connection
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_get_mtu(uint8_t connection, uint16_t *mtu);

/***************************************************************************//**
 *
 * Find attributes of a certain type from a local GATT database. The type is
 * usually given as a 16-bit or 128-bit UUID in little endian format.
 *
 * @param[in] start Search start handle
 * @param[in] type_len Length of data in @p type
 * @param[in] type The attribute type UUID
 * @param[out] attribute Attribute handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_find_attribute(uint16_t start,
                                             size_t type_len,
                                             const uint8_t* type,
                                             uint16_t *attribute);

/***************************************************************************//**
 *
 * Read the value of an attribute from a local GATT database. Only (maximum
 * BGAPI payload size - 3) amount of data can be read at once. The application
 * can continue reading with increased offset value if it receives (maximum
 * BGAPI payload size - 3) amount of data.
 *
 * @param[in] attribute Attribute handle
 * @param[in] offset Value offset
 * @param[in] max_value_size Size of output buffer passed in @p value
 * @param[out] value_len On return, set to the length of output data written to
 *   @p value
 * @param[out] value The attribute value
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_read_attribute_value(uint16_t attribute,
                                                   uint16_t offset,
                                                   size_t max_value_size,
                                                   size_t *value_len,
                                                   uint8_t *value);

/***************************************************************************//**
 *
 * Read the type of an attribute from a local GATT database. The type is a UUID,
 * usually 16 or 128 bits long in little endian format.
 *
 * @param[in] attribute Attribute handle
 * @param[in] max_type_size Size of output buffer passed in @p type
 * @param[out] type_len On return, set to the length of output data written to
 *   @p type
 * @param[out] type The attribute type UUID
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_read_attribute_type(uint16_t attribute,
                                                  size_t max_type_size,
                                                  size_t *type_len,
                                                  uint8_t *type);

/***************************************************************************//**
 *
 * Write the value of an attribute in the local GATT database. Writing the value
 * of a characteristic of the local GATT database will not trigger notifications
 * or indications to the remote GATT client if the characteristic has a property
 * to indicate or notify and the client has enabled notification or indication.
 * Notifications and indications are sent to the remote GATT client using @ref
 * sl_bt_gatt_server_send_notification or @ref sl_bt_gatt_server_send_indication
 * commands.
 *
 * @param[in] attribute Attribute handle
 * @param[in] offset Value offset
 * @param[in] value_len Length of data in @p value
 * @param[in] value Value
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_write_attribute_value(uint16_t attribute,
                                                    uint16_t offset,
                                                    size_t value_len,
                                                    const uint8_t* value);

/***************************************************************************//**
 *
 * Send a response to a @ref sl_bt_evt_gatt_server_user_read_request event. The
 * response needs to be sent within 30 seconds, otherwise no more GATT
 * transactions are allowed by the remote side. If attr_errorcode is set to 0,
 * the characteristic value is sent to the remote GATT client in the standard
 * way. Other attr_errorcode values will cause the local GATT server to send an
 * attribute protocol error response instead of the actual data. At most,
 * ATT_MTU - 1 amount of data can be sent at one time. The client will continue
 * reading by sending new read request with an increased offset value if it
 * receives ATT_MTU - 1 amount of data.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle received in the @ref
 *   sl_bt_evt_gatt_server_user_read_request event.
 * @param[in] att_errorcode Attribute protocol error code
 *     - <b>0:</b> No error
 *     - <b>Non-zero:</b> See Bluetooth specification, Host volume, Attribute
 *       Protocol, Error Codes table.
 * @param[in] value_len Length of data in @p value
 * @param[in] value Characteristic value to send to the GATT client. Ignored if
 *   att_errorcode is not 0.
 * @param[out] sent_len The length of data sent to the remote GATT client
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_send_user_read_response(uint8_t connection,
                                                      uint16_t characteristic,
                                                      uint8_t att_errorcode,
                                                      size_t value_len,
                                                      const uint8_t* value,
                                                      uint16_t *sent_len);

/***************************************************************************//**
 *
 * Send a response to a @ref sl_bt_evt_gatt_server_user_write_request event when
 * parameter @p att_opcode in the event is @ref sl_bt_gatt_write_request or @ref
 * sl_bt_gatt_execute_write_request (see @ref sl_bt_gatt_att_opcode_t). The
 * response needs to be sent within 30 seconds, otherwise no more GATT
 * transactions are allowed by the remote side. When responding to @ref
 * sl_bt_gatt_execute_write_request, the value of parameter @p characteristic is
 * ignored. If attr_errorcode is set to 0, the ATT protocol's write response is
 * sent to indicate to the remote GATT client that the write operation was
 * processed successfully. Other values will cause the local GATT server to send
 * an ATT protocol error response.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle received in the @ref
 *   sl_bt_evt_gatt_server_user_write_request event
 * @param[in] att_errorcode Attribute protocol error code
 *     - <b>0:</b> No error
 *     - <b>Non-zero:</b> See Bluetooth specification, Host volume, Attribute
 *       Protocol, Error Codes table.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_send_user_write_response(uint8_t connection,
                                                       uint16_t characteristic,
                                                       uint8_t att_errorcode);

/***************************************************************************//**
 *
 * <b>Deprecated</b> and replaced by @ref sl_bt_gatt_server_send_notification,
 * @ref sl_bt_gatt_server_send_indication and @ref sl_bt_gatt_server_notify_all
 * commands.
 *
 * A notification or indication is sent only if the client has enabled it by
 * setting the corresponding flag to the Client Characteristic Configuration
 * descriptor. If the Client Characteristic Configuration descriptor supports
 * both notifications and indications, the stack will always send a notification
 * even when the client has enabled both.
 *
 * Send notifications or indications to one or more remote GATT clients. At
 * most, ATT_MTU - 3 amount of data can be sent one time.
 *
 * A new indication to a GATT client can't be sent until an outstanding
 * indication procedure with the same client has completed. The procedure is
 * completed when a confirmation from the client is received. The confirmation
 * is indicated by @ref sl_bt_evt_gatt_server_characteristic_status.
 *
 * The error SL_STATUS_INVALID_STATE is returned if the characteristic does not
 * have the notification property, or if the client has not enabled the
 * notification. The same applies to the indication property, and in addition,
 * SL_STATUS_INVALID_STATE is returned if an indication procedure with the same
 * client is outstanding. Always check the response for this command for errors
 * before trying to send more data.
 *
 * @param[in] connection A handle of the connection over which the notification
 *   or indication is sent. Values:
 *     - <b>0xff:</b> Sends notification or indication to all connected devices.
 *     - <b>Other:</b> Connection handle
 * @param[in] characteristic Characteristic handle
 * @param[in] value_len Length of data in @p value
 * @param[in] value Value to be notified or indicated
 * @param[out] sent_len The length of data sent if only one connected device is
 *   the receiver; otherwise an unused value.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
SL_BGAPI_DEPRECATED sl_status_t sl_bt_gatt_server_send_characteristic_notification(uint8_t connection,
                                                               uint16_t characteristic,
                                                               size_t value_len,
                                                               const uint8_t* value,
                                                               uint16_t *sent_len);

/***************************************************************************//**
 *
 * Send a notification to a remote GATT client. At most ATT_MTU - 3 amount of
 * data can be sent in a notification. An error SL_STATUS_COMMAND_TOO_LONG is
 * returned if the given value length exceeds ATT_MTU - 3.
 *
 * A notification is sent only if the client has enabled it by setting the
 * corresponding flag to the Client Characteristic Configuration descriptor. The
 * error SL_STATUS_INVALID_PARAMETER is returned if the characteristic does not
 * have the notification property. The error SL_STATUS_INVALID_STATE is returned
 * if the client has not enabled the notification.
 *
 * @param[in] connection A handle of the connection over which the notification
 *   is sent.
 * @param[in] characteristic Characteristic handle
 * @param[in] value_len Length of data in @p value
 * @param[in] value Value to be notified
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_send_notification(uint8_t connection,
                                                uint16_t characteristic,
                                                size_t value_len,
                                                const uint8_t* value);

/***************************************************************************//**
 *
 * Send an indication to a remote GATT client. At most, ATT_MTU - 3 amount of
 * data can be sent in an indication. An error SL_STATUS_COMMAND_TOO_LONG is
 * returned if the given value length exceeds ATT_MTU - 3.
 *
 * An indication is sent only if the client has enabled it by setting the
 * corresponding flag to the Client Characteristic Configuration descriptor. The
 * error SL_STATUS_INVALID_PARAMETER is returned if the characteristic does not
 * have the indication property. The error SL_STATUS_INVALID_STATE is returned
 * if the client has not enabled the indication.
 *
 * A new indication to a GATT client can't be sent until an outstanding
 * indication procedure with the same client has completed. The procedure is
 * completed when a confirmation from the client is received. The confirmation
 * is indicated by @ref sl_bt_evt_gatt_server_characteristic_status.
 *
 * The error SL_STATUS_IN_PROGRESS is returned if an indication procedure with
 * the same client is outstanding. Always wait for confirmation for previous
 * indication before sending a new indication.
 *
 * @param[in] connection A handle of the connection over which the indication is
 *   sent.
 * @param[in] characteristic Characteristic handle
 * @param[in] value_len Length of data in @p value
 * @param[in] value Value to be indicated
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_server_characteristic_status - This event is
 *     triggered after the confirmation from the client is received.
 *   - @ref sl_bt_evt_gatt_server_indication_timeout - This event indicates
 *     confirmation from the remote GATT client has not been received within 30
 *     seconds after an indication was sent. Further GATT transactions over this
 *     connection are not allowed by the stack.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_send_indication(uint8_t connection,
                                              uint16_t characteristic,
                                              size_t value_len,
                                              const uint8_t* value);

/***************************************************************************//**
 *
 * Send notifications or indications to all connected remote GATT clients. At
 * most ATT_MTU - 3 amount of data can be sent in a notification or indication.
 * If the given value length exceeds the limit, first ATT_MTU - 3 bytes will be
 * sent without error, and rest of data will be ignored.
 *
 * A notification or indication is sent only if the client has enabled it by
 * setting the corresponding flag to the Client Characteristic Configuration
 * descriptor. If the Client Characteristic Configuration descriptor supports
 * both notifications and indications, the stack will always send a notification
 * even when the client has enabled both.
 *
 * A new indication to a GATT client can't be sent until an outstanding
 * indication procedure with the same client has completed, and the operation
 * will continue for the next client. The procedure is completed when a
 * confirmation from the client is received. The confirmation is indicated by
 * @ref sl_bt_evt_gatt_server_characteristic_status.
 *
 * @param[in] characteristic Characteristic handle
 * @param[in] value_len Length of data in @p value
 * @param[in] value Value to be notified or indicated
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_notify_all(uint16_t characteristic,
                                         size_t value_len,
                                         const uint8_t* value);

/***************************************************************************//**
 *
 * Read client characteristic configuration of a remote GATT client.
 *
 * @param[in] connection A handle of the connection to a remote client.
 * @param[in] characteristic Characteristic handle
 * @param[out] client_config_flags Enum @ref
 *   sl_bt_gatt_server_client_configuration_t. Client characteristic
 *   configuration of a remote client.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_read_client_configuration(uint8_t connection,
                                                        uint16_t characteristic,
                                                        uint16_t *client_config_flags);

/***************************************************************************//**
 *
 * Send a response to a @ref sl_bt_evt_gatt_server_user_write_request event when
 * parameter @p att_opcode in the event is @ref sl_bt_gatt_prepare_write_request
 * (see @ref sl_bt_gatt_att_opcode_t). The response needs to be sent within 30
 * seconds, otherwise no more GATT transactions are allowed by the remote side.
 * If @p att_errorcode is set to 0, the ATT protocol's prepare write response is
 * sent to indicate to the remote GATT client that the write operation was
 * processed successfully. Other values will cause the local GATT server to send
 * an ATT protocol error response. The application should set values of
 * parameters @p offset and @p value to identical values from the @ref
 * sl_bt_evt_gatt_server_user_write_request event, the values will be verified
 * on the client side in case the request is a reliable write (by Bluetooth Core
 * Specification Volume 3, Part G, 4.9.5).
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 * @param[in] att_errorcode Attribute protocol error code
 *     - <b>0:</b> No error
 *     - <b>Non-zero:</b> See Bluetooth specification, Host volume, Attribute
 *       Protocol, Error Codes table.
 * @param[in] offset Value offset
 * @param[in] value_len Length of data in @p value
 * @param[in] value Value
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_send_user_prepare_write_response(uint8_t connection,
                                                               uint16_t characteristic,
                                                               uint8_t att_errorcode,
                                                               uint16_t offset,
                                                               size_t value_len,
                                                               const uint8_t* value);

/***************************************************************************//**
 *
 * Reset capabilities that should be enabled by the GATT database. A service is
 * visible to remote GATT clients if at least one of its capabilities is
 * enabled. The same applies to a characteristic and its attributes. Capability
 * identifiers and their corresponding bit flag values are in the auto-generated
 * database header file. See UG118: Blue Gecko Bluetooth Profile Toolkit
 * Developer's Guide for how to declare capabilities in the GATT database.
 *
 * Changing the capabilities of a database effectively causes a database change
 * (attributes being added or removed) from a remote GATT client point of view.
 * If the database has a Generic Attribute service and Service Changed
 * characteristic, the stack will monitor the local database change status and
 * manage service changed indications for a GATT client that has enabled the
 * indication configuration of the Service Changed characteristic.
 *
 * @param[in] caps Bit flags of capabilities to reset. Value 0 sets the default
 *   database capabilities.
 * @param[in] reserved Use the value 0 on this reserved field. Do not use
 *   none-zero values because they are reserved for future use.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_set_capabilities(uint32_t caps,
                                               uint32_t reserved);

/***************************************************************************//**
 *
 * Enable additional capabilities in the local GATT database. Already enabled
 * capabilities keep unchanged after this command. See @ref
 * sl_bt_gatt_server_set_capabilities for more information.
 *
 * @param[in] caps Capabilities to enable
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_enable_capabilities(uint32_t caps);

/***************************************************************************//**
 *
 * Disable the given capabilities in the local GATT database. See @ref
 * sl_bt_gatt_server_set_capabilities for more information.
 *
 * @param[in] caps Capabilities to disable
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_disable_capabilities(uint32_t caps);

/***************************************************************************//**
 *
 * Get capabilities currently enabled in the local GATT database.
 *
 * @param[out] caps Enabled capabilities
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_get_enabled_capabilities(uint32_t *caps);

/***************************************************************************//**
 *
 * Read client supported features of a remote GATT client.
 *
 * @param[in] connection A handle of the connection to a remote client.
 * @param[out] client_features Bit field describing client supported features of
 *   a remote client. See Bluetooth specification Vol 3, Part G, 7.2 for the
 *   values.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_read_client_supported_features(uint8_t connection,
                                                             uint8_t *client_features);

/** @} */ // end addtogroup sl_bt_gatt_server

/**
 * @addtogroup sl_bt_nvm NVM
 * @{
 *
 * @brief NVM
 *
 * Provide an interface to manage user data objects (key/value pairs) in the
 * flash memory. User data stored within the flash memory is persistent across
 * reset and power cycling of the device. Because Bluetooth bondings are also
 * stored in the flash area, in addition to the flash storage size, the space
 * available for user data also depends on the number of bondings the device has
 * at the time.
 *
 * On EFR32[B|M]G1x devices, either PS Store or NVM3 data storage driver can be
 * used. PS Store is supported by the Bluetooth stack only. Using NVM3 is
 * recommended if the device needs to support Dynamic Multiple Protocol (DMP)
 * cases. On EFR32[B|M]G2x devices, only NVM3 is supported. When NVM3 is used,
 * applications can also use the NVM3 APIs directly.
 *
 * In PS Store, the flash storage size is fixed at 2048 bytes. The maximum data
 * object size associated to a key is 56 bytes. A Bluetooth bonding uses at
 * maximum 138 bytes for secure connections and 174 bytes for legacy pairing.
 *
 * In NVM3, the flash store size is configurable and the minimum is 3 flash
 * pages. The maximum data object size is configurable up to 4096 bytes. A
 * Bluetooth bonding uses maximum 110 bytes for secure connections and 138 bytes
 * for legacy pairing. For more details, see AN1135 "Using Third Generation
 * NonVolatile Memory (NVM3) Data Storage".
 */

/* Command and Response IDs */
#define sl_bt_cmd_nvm_save_id                                        0x020d0020
#define sl_bt_cmd_nvm_load_id                                        0x030d0020
#define sl_bt_cmd_nvm_erase_id                                       0x040d0020
#define sl_bt_cmd_nvm_erase_all_id                                   0x010d0020
#define sl_bt_rsp_nvm_save_id                                        0x020d0020
#define sl_bt_rsp_nvm_load_id                                        0x030d0020
#define sl_bt_rsp_nvm_erase_id                                       0x040d0020
#define sl_bt_rsp_nvm_erase_all_id                                   0x010d0020

/**
 * @addtogroup sl_bt_nvm_keys Defined Keys
 * @{
 *
 * Define keys
 */

/** Crystal tuning value override */
#define SL_BT_NVM_KEY_CTUNE 0x32      

/** @} */ // end Defined Keys

/***************************************************************************//**
 *
 * Store a value into the specified NVM key. Allowed NVM keys are in range from
 * 0x4000 to 0x407F. At most, 56 bytes user data can be stored in one NVM key.
 * The error code 0x018a (command_too_long) is returned if the value data is
 * more than 56 bytes.
 *
 * @param[in] key NVM key
 * @param[in] value_len Length of data in @p value
 * @param[in] value Value to store into the specified NVM key
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_nvm_save(uint16_t key,
                           size_t value_len,
                           const uint8_t* value);

/***************************************************************************//**
 *
 * Retrieve the value of the specified NVM key.
 *
 * @param[in] key NVM key of the value to be retrieved
 * @param[in] max_value_size Size of output buffer passed in @p value
 * @param[out] value_len On return, set to the length of output data written to
 *   @p value
 * @param[out] value The returned value of the specified NVM key
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_nvm_load(uint16_t key,
                           size_t max_value_size,
                           size_t *value_len,
                           uint8_t *value);

/***************************************************************************//**
 *
 * Delete a single NVM key and its value from the persistent store.
 *
 * @param[in] key NVM key to delete
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_nvm_erase(uint16_t key);

/***************************************************************************//**
 *
 * Delete all NVM keys and their corresponding values.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_nvm_erase_all();

/** @} */ // end addtogroup sl_bt_nvm

/**
 * @addtogroup sl_bt_test Testing Commands
 * @{
 *
 * @brief Testing Commands
 */

/* Command and Response IDs */
#define sl_bt_cmd_test_dtm_tx_id                                     0x000e0020
#define sl_bt_cmd_test_dtm_tx_v4_id                                  0x030e0020
#define sl_bt_cmd_test_dtm_rx_id                                     0x010e0020
#define sl_bt_cmd_test_dtm_end_id                                    0x020e0020
#define sl_bt_rsp_test_dtm_tx_id                                     0x000e0020
#define sl_bt_rsp_test_dtm_tx_v4_id                                  0x030e0020
#define sl_bt_rsp_test_dtm_rx_id                                     0x010e0020
#define sl_bt_rsp_test_dtm_end_id                                    0x020e0020

/**
 * @brief 
                Test packet types supported by the stack
            
 */
typedef enum
{
  sl_bt_test_pkt_prbs9    = 0x0,  /**< (0x0) PRBS9 packet payload */
  sl_bt_test_pkt_11110000 = 0x1,  /**< (0x1) 11110000 packet payload */
  sl_bt_test_pkt_10101010 = 0x2,  /**< (0x2) 10101010 packet payload */
  sl_bt_test_pkt_11111111 = 0x4,  /**< (0x4) 11111111 packet payload */
  sl_bt_test_pkt_00000000 = 0x5,  /**< (0x5) 00000000 packet payload */
  sl_bt_test_pkt_00001111 = 0x6,  /**< (0x6) 00001111 packet payload */
  sl_bt_test_pkt_01010101 = 0x7,  /**< (0x7) 01010101 packet payload */
  sl_bt_test_pkt_pn9      = 0xfd, /**< (0xfd) PN9 continuously modulated output */
  sl_bt_test_pkt_carrier  = 0xfe  /**< (0xfe) Unmodulated carrier */
} sl_bt_test_packet_type_t;

/**
 * @brief Test PHY types
 */
typedef enum
{
  sl_bt_test_phy_1m   = 0x1, /**< (0x1) 1M PHY */
  sl_bt_test_phy_2m   = 0x2, /**< (0x2) 2M PHY */
  sl_bt_test_phy_125k = 0x3, /**< (0x3) 125k Coded PHY */
  sl_bt_test_phy_500k = 0x4  /**< (0x4) 500k Coded PHY */
} sl_bt_test_phy_t;

/**
 * @addtogroup sl_bt_evt_test_dtm_completed sl_bt_evt_test_dtm_completed
 * @{
 * @brief Indicates that the radio has processed a test start or end command
 *
 * The @p result parameter indicates the success of the command.
 *
 * After the receiver or transmitter test is stopped, the @p number_of_packets
 * parameter in this event indicates the number of received or transmitted
 * packets.
 */

/** @brief Identifier of the dtm_completed event */
#define sl_bt_evt_test_dtm_completed_id                              0x000e00a0

/***************************************************************************//**
 * @brief Data structure of the dtm_completed event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_test_dtm_completed_s
{
  uint16_t result;            /**< Command result */
  uint16_t number_of_packets; /**< Number of packets

                                   Only valid for @ref sl_bt_test_dtm_end
                                   command. */
});

typedef struct sl_bt_evt_test_dtm_completed_s sl_bt_evt_test_dtm_completed_t;

/** @} */ // end addtogroup sl_bt_evt_test_dtm_completed

/***************************************************************************//**
 *
 * <b>Deprecated</b> and replaced by @ref sl_bt_test_dtm_tx_v4 command.
 *
 * Start a transmitter test against a separate Bluetooth tester device. When the
 * command is processed by the radio, a @ref sl_bt_evt_test_dtm_completed event
 * is triggered. This event indicates whether the test started successfully.
 *
 * In the transmitter test, the device sends packets continuously with a fixed
 * interval. The type and length of each packet is set by @p packet_type and @p
 * length parameters. The parameter @p phy specifies which PHY is used to
 * transmit the packets. All devices support at least 1M PHY. A special packet
 * type, @p test_pkt_carrier, can be used to transmit continuous unmodulated
 * carrier. The @p length field is ignored in this mode.
 *
 * The test may be stopped using the @ref sl_bt_test_dtm_end command.
 *
 * @param[in] packet_type Enum @ref sl_bt_test_packet_type_t. Packet type to
 *   transmit. Values:
 *     - <b>sl_bt_test_pkt_prbs9 (0x0):</b> PRBS9 packet payload
 *     - <b>sl_bt_test_pkt_11110000 (0x1):</b> 11110000 packet payload
 *     - <b>sl_bt_test_pkt_10101010 (0x2):</b> 10101010 packet payload
 *     - <b>sl_bt_test_pkt_11111111 (0x4):</b> 11111111 packet payload
 *     - <b>sl_bt_test_pkt_00000000 (0x5):</b> 00000000 packet payload
 *     - <b>sl_bt_test_pkt_00001111 (0x6):</b> 00001111 packet payload
 *     - <b>sl_bt_test_pkt_01010101 (0x7):</b> 01010101 packet payload
 *     - <b>sl_bt_test_pkt_pn9 (0xfd):</b> PN9 continuously modulated output
 *     - <b>sl_bt_test_pkt_carrier (0xfe):</b> Unmodulated carrier
 * @param[in] length @parblock
 *   Packet length in bytes
 *
 *   <b>Range:</b> 0-255
 *   @endparblock
 * @param[in] channel @parblock
 *   Bluetooth channel
 *
 *   <b>Range:</b> 0-39
 *
 *   Channel is (F - 2402) / 2,
 *
 *   where F is frequency in MHz
 *   @endparblock
 * @param[in] phy Enum @ref sl_bt_test_phy_t. PHY to use. Values:
 *     - <b>sl_bt_test_phy_1m (0x1):</b> 1M PHY
 *     - <b>sl_bt_test_phy_2m (0x2):</b> 2M PHY
 *     - <b>sl_bt_test_phy_125k (0x3):</b> 125k Coded PHY
 *     - <b>sl_bt_test_phy_500k (0x4):</b> 500k Coded PHY
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_test_dtm_completed - This event is received when the
 *     command is processed.
 *
 ******************************************************************************/
SL_BGAPI_DEPRECATED sl_status_t sl_bt_test_dtm_tx(uint8_t packet_type,
                              uint8_t length,
                              uint8_t channel,
                              uint8_t phy);

/***************************************************************************//**
 *
 * Start a transmitter test against a separate Bluetooth tester device. When the
 * command is processed by the radio, a @ref sl_bt_evt_test_dtm_completed event
 * is triggered. This event indicates whether the test started successfully.
 *
 * In the transmitter test, the device sends packets continuously with a fixed
 * interval. The type and length of each packet is set by @p packet_type and @p
 * length parameters. The parameter @p phy specifies which PHY is used to
 * transmit the packets. All devices support at least 1M PHY. A special packet
 * type, <b>test_pkt_carrier</b> , can be used to transmit continuous
 * unmodulated carrier. The <b>length</b> field is ignored in this mode.
 *
 * Stop the test using the @ref sl_bt_test_dtm_end command.
 *
 * @param[in] packet_type Enum @ref sl_bt_test_packet_type_t. Packet type to
 *   transmit. Values:
 *     - <b>sl_bt_test_pkt_prbs9 (0x0):</b> PRBS9 packet payload
 *     - <b>sl_bt_test_pkt_11110000 (0x1):</b> 11110000 packet payload
 *     - <b>sl_bt_test_pkt_10101010 (0x2):</b> 10101010 packet payload
 *     - <b>sl_bt_test_pkt_11111111 (0x4):</b> 11111111 packet payload
 *     - <b>sl_bt_test_pkt_00000000 (0x5):</b> 00000000 packet payload
 *     - <b>sl_bt_test_pkt_00001111 (0x6):</b> 00001111 packet payload
 *     - <b>sl_bt_test_pkt_01010101 (0x7):</b> 01010101 packet payload
 *     - <b>sl_bt_test_pkt_pn9 (0xfd):</b> PN9 continuously modulated output
 *     - <b>sl_bt_test_pkt_carrier (0xfe):</b> Unmodulated carrier
 * @param[in] length @parblock
 *   Packet length in bytes
 *
 *   <b>Range:</b> 0-255
 *   @endparblock
 * @param[in] channel @parblock
 *   Bluetooth channel
 *
 *   <b>Range:</b> 0-39
 *
 *   Channel is (F - 2402) / 2,
 *
 *   where F is frequency in MHz
 *   @endparblock
 * @param[in] phy Enum @ref sl_bt_test_phy_t. PHY to use. Values:
 *     - <b>sl_bt_test_phy_1m (0x1):</b> 1M PHY
 *     - <b>sl_bt_test_phy_2m (0x2):</b> 2M PHY
 *     - <b>sl_bt_test_phy_125k (0x3):</b> 125k Coded PHY
 *     - <b>sl_bt_test_phy_500k (0x4):</b> 500k Coded PHY
 * @param[in] power_level @parblock
 *   TX power level in unit dBm. Values:
 *     - <b>-127 to +20:</b> Use specified or the nearest TX power level. The
 *       minimum -127 dBm is specified in the Bluetooth specification. However,
 *       a device may not support this low TX power. In addition, only some
 *       devices support 20 dBm TX power.
 *     - <b>0x7E:</b> Use minimum TX power level the device supports.
 *     - <b>0x7F:</b> Use the smallest of the maximum TX power level the device
 *       supports and the global maximum TX power setting in stack.
 *
 *   For continious unmodulated carrier mode, the values are set in deci-dBm
 *   values. In case, if the value exceeds the range of power level value,
 *   allowed by the device, the command will adjust the power level to the
 *   closest minimum or maximum value.
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_test_dtm_completed - This event is received when the
 *     command is processed.
 *
 ******************************************************************************/
sl_status_t sl_bt_test_dtm_tx_v4(uint8_t packet_type,
                                 uint8_t length,
                                 uint8_t channel,
                                 uint8_t phy,
                                 int8_t power_level);

/***************************************************************************//**
 *
 * Start a receiver test against a separate Bluetooth tester device. When the
 * command is processed by the radio, a @ref sl_bt_evt_test_dtm_completed event
 * is triggered. This event indicates whether the test started successfully.
 *
 * Parameter @p phy specifies which PHY is used to receive the packets. All
 * devices support at least 1M PHY.
 *
 * The test may be stopped using @ref sl_bt_test_dtm_end command. This will
 * trigger another @ref sl_bt_evt_test_dtm_completed event, which carries the
 * number of packets received during the test.
 *
 * @param[in] channel @parblock
 *   Bluetooth channel
 *
 *   <b>Range:</b> 0-39
 *
 *   Channel is (F - 2402) / 2,
 *
 *   where F is frequency in MHz
 *   @endparblock
 * @param[in] phy Enum @ref sl_bt_test_phy_t. PHY to use. Values:
 *     - <b>sl_bt_test_phy_1m (0x1):</b> 1M PHY
 *     - <b>sl_bt_test_phy_2m (0x2):</b> 2M PHY
 *     - <b>sl_bt_test_phy_125k (0x3):</b> 125k Coded PHY
 *     - <b>sl_bt_test_phy_500k (0x4):</b> 500k Coded PHY
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_test_dtm_completed - This event is received when the
 *     command is processed.
 *
 ******************************************************************************/
sl_status_t sl_bt_test_dtm_rx(uint8_t channel, uint8_t phy);

/***************************************************************************//**
 *
 * End a transmitter or a receiver test. When the command is processed by the
 * radio and the test has ended, a @ref sl_bt_evt_test_dtm_completed event is
 * triggered.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_test_dtm_completed - Received when the command is
 *     processed by the radio and the test has ended.
 *
 ******************************************************************************/
sl_status_t sl_bt_test_dtm_end();

/** @} */ // end addtogroup sl_bt_test

/**
 * @addtogroup sl_bt_sm Security Manager
 * @{
 *
 * @brief Security Manager
 *
 * The commands in this class manage Bluetooth security, including commands for
 * starting and stopping encryption and commands for management of all bonding
 * operations.
 *
 * Use the following procedure to bond with a remote device:
 *   - Use the command @ref sl_bt_sm_configure to configure security
 *     requirements and I/O capabilities of this device.
 *   - Use the command @ref sl_bt_sm_set_bondable_mode to set this device into
 *     bondable mode.
 *   - Use the command @ref sl_bt_connection_open to open a connection to the
 *     remote device.
 *   - After the connection is open, use command @ref sl_bt_sm_increase_security
 *     to encrypt the connection. This will also start the bonding process.
 *
 * Use the following procedure to respond to the bonding initiated by a remote
 * device:
 *   - Use the command @ref sl_bt_sm_configure to configure security
 *     requirements and I/O capabilities of this device.
 *   - Use the command @ref sl_bt_sm_set_bondable_mode to set this device into
 *     bondable mode.
 *   - Use the command @ref sl_bt_advertiser_start to set this device into
 *     advertising and connectable mode.
 *   - Open a connection to this device from the remote device.
 *   - After the connection is open, start the bonding process on the remote
 *     device.
 *
 * If MITM is required, the application needs to display or ask the user to
 * enter a passkey during the process. See events @ref
 * sl_bt_evt_sm_passkey_display and @ref sl_bt_evt_sm_passkey_request for more
 * information.
 */

/* Command and Response IDs */
#define sl_bt_cmd_sm_configure_id                                    0x010f0020
#define sl_bt_cmd_sm_set_minimum_key_size_id                         0x140f0020
#define sl_bt_cmd_sm_set_debug_mode_id                               0x0f0f0020
#define sl_bt_cmd_sm_add_to_whitelist_id                             0x130f0020
#define sl_bt_cmd_sm_store_bonding_configuration_id                  0x020f0020
#define sl_bt_cmd_sm_set_bondable_mode_id                            0x000f0020
#define sl_bt_cmd_sm_set_passkey_id                                  0x100f0020
#define sl_bt_cmd_sm_set_oob_data_id                                 0x0a0f0020
#define sl_bt_cmd_sm_use_sc_oob_id                                   0x110f0020
#define sl_bt_cmd_sm_set_sc_remote_oob_data_id                       0x120f0020
#define sl_bt_cmd_sm_increase_security_id                            0x040f0020
#define sl_bt_cmd_sm_enter_passkey_id                                0x080f0020
#define sl_bt_cmd_sm_passkey_confirm_id                              0x090f0020
#define sl_bt_cmd_sm_bonding_confirm_id                              0x0e0f0020
#define sl_bt_cmd_sm_list_all_bondings_id                            0x0b0f0020
#define sl_bt_cmd_sm_delete_bonding_id                               0x060f0020
#define sl_bt_cmd_sm_delete_bondings_id                              0x070f0020
#define sl_bt_cmd_sm_get_bonding_handles_id                          0x150f0020
#define sl_bt_cmd_sm_get_bonding_details_id                          0x160f0020
#define sl_bt_cmd_sm_find_bonding_by_address_id                      0x170f0020
#define sl_bt_cmd_sm_set_bonding_key_id                              0x180f0020
#define sl_bt_cmd_sm_set_legacy_oob_id                               0x190f0020
#define sl_bt_cmd_sm_set_oob_id                                      0x1a0f0020
#define sl_bt_cmd_sm_set_remote_oob_id                               0x1b0f0020
#define sl_bt_rsp_sm_configure_id                                    0x010f0020
#define sl_bt_rsp_sm_set_minimum_key_size_id                         0x140f0020
#define sl_bt_rsp_sm_set_debug_mode_id                               0x0f0f0020
#define sl_bt_rsp_sm_add_to_whitelist_id                             0x130f0020
#define sl_bt_rsp_sm_store_bonding_configuration_id                  0x020f0020
#define sl_bt_rsp_sm_set_bondable_mode_id                            0x000f0020
#define sl_bt_rsp_sm_set_passkey_id                                  0x100f0020
#define sl_bt_rsp_sm_set_oob_data_id                                 0x0a0f0020
#define sl_bt_rsp_sm_use_sc_oob_id                                   0x110f0020
#define sl_bt_rsp_sm_set_sc_remote_oob_data_id                       0x120f0020
#define sl_bt_rsp_sm_increase_security_id                            0x040f0020
#define sl_bt_rsp_sm_enter_passkey_id                                0x080f0020
#define sl_bt_rsp_sm_passkey_confirm_id                              0x090f0020
#define sl_bt_rsp_sm_bonding_confirm_id                              0x0e0f0020
#define sl_bt_rsp_sm_list_all_bondings_id                            0x0b0f0020
#define sl_bt_rsp_sm_delete_bonding_id                               0x060f0020
#define sl_bt_rsp_sm_delete_bondings_id                              0x070f0020
#define sl_bt_rsp_sm_get_bonding_handles_id                          0x150f0020
#define sl_bt_rsp_sm_get_bonding_details_id                          0x160f0020
#define sl_bt_rsp_sm_find_bonding_by_address_id                      0x170f0020
#define sl_bt_rsp_sm_set_bonding_key_id                              0x180f0020
#define sl_bt_rsp_sm_set_legacy_oob_id                               0x190f0020
#define sl_bt_rsp_sm_set_oob_id                                      0x1a0f0020
#define sl_bt_rsp_sm_set_remote_oob_id                               0x1b0f0020

/**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 * @brief 
                These values define the bonding keys of the bonded device stored in the persistent store.
            
 */
typedef enum
{
  sl_bt_sm_bonding_key_remote_ltk = 0x1, /**< (0x1) LTK used as central device,
                                              always used when paired using
                                              secure connections and local LTK
                                              does not exist */
  sl_bt_sm_bonding_key_local_ltk  = 0x2, /**< (0x2) LTK used as peripheral
                                              device when using legacy pairing,
                                              only used with secure connections
                                              pairing if key exists */
  sl_bt_sm_bonding_key_irk        = 0x3  /**< (0x3) Identity resolving key for
                                              resolvable private addresses */
} sl_bt_sm_bonding_key_t;
/** @endcond */ // end restricted enum type

/**
 * @brief 
                These values define the security management related I/O capabilities supported by the device.
            
 */
typedef enum
{
  sl_bt_sm_io_capability_displayonly     = 0x0, /**< (0x0) Display Only */
  sl_bt_sm_io_capability_displayyesno    = 0x1, /**< (0x1) Display with
                                                     Yes/No-buttons */
  sl_bt_sm_io_capability_keyboardonly    = 0x2, /**< (0x2) Keyboard Only */
  sl_bt_sm_io_capability_noinputnooutput = 0x3, /**< (0x3) No Input and No
                                                     Output */
  sl_bt_sm_io_capability_keyboarddisplay = 0x4  /**< (0x4) Display with Keyboard */
} sl_bt_sm_io_capability_t;

/**
 * @addtogroup sl_bt_evt_sm_passkey_display sl_bt_evt_sm_passkey_display
 * @{
 * @brief Indicates a request to display the passkey to the user.
 */

/** @brief Identifier of the passkey_display event */
#define sl_bt_evt_sm_passkey_display_id                              0x000f00a0

/***************************************************************************//**
 * @brief Data structure of the passkey_display event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sm_passkey_display_s
{
  uint8_t  connection; /**< Connection handle */
  uint32_t passkey;    /**< Passkey. Range: 0 to 999999.
                              - NOTE! When displaying the passkey to the user,
                                prefix the number with zeros to obtain a 6 digit
                                number
                              - Example: Passkey value is 42
                              - Number to display to the user is 000042 */
});

typedef struct sl_bt_evt_sm_passkey_display_s sl_bt_evt_sm_passkey_display_t;

/** @} */ // end addtogroup sl_bt_evt_sm_passkey_display

/**
 * @addtogroup sl_bt_evt_sm_passkey_request sl_bt_evt_sm_passkey_request
 * @{
 * @brief Indicates a request for the passkey prompt displayed on the remote
 * device
 *
 * Use the command @ref sl_bt_sm_enter_passkey to input the passkey value.
 */

/** @brief Identifier of the passkey_request event */
#define sl_bt_evt_sm_passkey_request_id                              0x010f00a0

/***************************************************************************//**
 * @brief Data structure of the passkey_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sm_passkey_request_s
{
  uint8_t connection; /**< Connection handle */
});

typedef struct sl_bt_evt_sm_passkey_request_s sl_bt_evt_sm_passkey_request_t;

/** @} */ // end addtogroup sl_bt_evt_sm_passkey_request

/**
 * @addtogroup sl_bt_evt_sm_confirm_passkey sl_bt_evt_sm_confirm_passkey
 * @{
 * @brief Indicates a request for passkey display and confirmation by the user
 *
 * Use the command @ref sl_bt_sm_passkey_confirm to accept or reject the
 * displayed passkey.
 */

/** @brief Identifier of the confirm_passkey event */
#define sl_bt_evt_sm_confirm_passkey_id                              0x020f00a0

/***************************************************************************//**
 * @brief Data structure of the confirm_passkey event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sm_confirm_passkey_s
{
  uint8_t  connection; /**< Connection handle */
  uint32_t passkey;    /**< Passkey. Range: 0 to 999999.
                              - NOTE! When displaying the passkey to the user,
                                prefix the number with zeros to obtain a 6 digit
                                number
                              - Example: Passkey value is 42
                              - Number to display to the user is 000042 */
});

typedef struct sl_bt_evt_sm_confirm_passkey_s sl_bt_evt_sm_confirm_passkey_t;

/** @} */ // end addtogroup sl_bt_evt_sm_confirm_passkey

/**
 * @addtogroup sl_bt_evt_sm_bonded sl_bt_evt_sm_bonded
 * @{
 * @brief Triggered after the pairing or bonding procedure is successfully
 * completed.
 */

/** @brief Identifier of the bonded event */
#define sl_bt_evt_sm_bonded_id                                       0x030f00a0

/***************************************************************************//**
 * @brief Data structure of the bonded event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sm_bonded_s
{
  uint8_t connection;    /**< Connection handle */
  uint8_t bonding;       /**< Bonding handle. Values:
                                - <b>SL_BT_INVALID_BONDING_HANDLE (0xff):</b>
                                  Pairing completed without bonding - the
                                  pairing key will be discarded after
                                  disconnection.
                                - <b>Other:</b> Procedure completed, pairing key
                                  stored with given bonding handle */
  uint8_t security_mode; /**< Enum @ref sl_bt_connection_security_t. Connection
                              security mode. Values:
                                - <b>sl_bt_connection_mode1_level2 (0x1):</b>
                                  Unauthenticated pairing with encryption
                                - <b>sl_bt_connection_mode1_level3 (0x2):</b>
                                  Authenticated pairing with encryption
                                - <b>sl_bt_connection_mode1_level4 (0x3):</b>
                                  Authenticated Secure Connections pairing with
                                  encryption using a 128-bit strength encryption
                                  key */
});

typedef struct sl_bt_evt_sm_bonded_s sl_bt_evt_sm_bonded_t;

/** @} */ // end addtogroup sl_bt_evt_sm_bonded

/**
 * @addtogroup sl_bt_evt_sm_bonding_failed sl_bt_evt_sm_bonding_failed
 * @{
 * @brief This event is triggered if the pairing or bonding procedure fails.
 */

/** @brief Identifier of the bonding_failed event */
#define sl_bt_evt_sm_bonding_failed_id                               0x040f00a0

/***************************************************************************//**
 * @brief Data structure of the bonding_failed event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sm_bonding_failed_s
{
  uint8_t  connection; /**< Connection handle */
  uint16_t reason;     /**< Describes error that occurred */
});

typedef struct sl_bt_evt_sm_bonding_failed_s sl_bt_evt_sm_bonding_failed_t;

/** @} */ // end addtogroup sl_bt_evt_sm_bonding_failed

/**
 * @addtogroup sl_bt_evt_sm_confirm_bonding sl_bt_evt_sm_confirm_bonding
 * @{
 * @brief Indicates a user request to display that the new bonding request is
 * received and for the user to confirm the request
 *
 * Use the command @ref sl_bt_sm_bonding_confirm to accept or reject the bonding
 * request.
 */

/** @brief Identifier of the confirm_bonding event */
#define sl_bt_evt_sm_confirm_bonding_id                              0x090f00a0

/***************************************************************************//**
 * @brief Data structure of the confirm_bonding event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sm_confirm_bonding_s
{
  uint8_t connection;     /**< Connection handle */
  uint8_t bonding_handle; /**< Bonding handle for the request. Range: 0 to 31,
                               or SL_BT_INVALID_BONDING_HANDLE (0xff).
                                 - NOTE! When the bonding handle is anything
                                   other than SL_BT_INVALID_BONDING_HANDLE
                                   (0xff), a bonding already exists for this
                                   connection. Overwriting the existing bonding
                                   is a potential security risk. */
});

typedef struct sl_bt_evt_sm_confirm_bonding_s sl_bt_evt_sm_confirm_bonding_t;

/** @} */ // end addtogroup sl_bt_evt_sm_confirm_bonding

/**
 * @addtogroup sl_bt_evt_sm_list_bonding_entry sl_bt_evt_sm_list_bonding_entry
 * @{
 * @brief Triggered by the command @ref sl_bt_sm_list_all_bondings if bondings
 * exist in the local database
 */

/** @brief Identifier of the list_bonding_entry event */
#define sl_bt_evt_sm_list_bonding_entry_id                           0x050f00a0

/***************************************************************************//**
 * @brief Data structure of the list_bonding_entry event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sm_list_bonding_entry_s
{
  uint8_t bonding;      /**< Bonding handle */
  bd_addr address;      /**< Bluetooth address of the remote device */
  uint8_t address_type; /**< Enum @ref sl_bt_gap_address_type_t. Address type.
                             Values:
                               - <b>sl_bt_gap_public_address (0x0):</b> Public
                                 device address
                               - <b>sl_bt_gap_static_address (0x1):</b> Static
                                 device address */
});

typedef struct sl_bt_evt_sm_list_bonding_entry_s sl_bt_evt_sm_list_bonding_entry_t;

/** @} */ // end addtogroup sl_bt_evt_sm_list_bonding_entry

/**
 * @addtogroup sl_bt_evt_sm_list_all_bondings_complete sl_bt_evt_sm_list_all_bondings_complete
 * @{
 * @brief Triggered by the @ref sl_bt_sm_list_all_bondings and follows @ref
 * sl_bt_evt_sm_list_bonding_entry events
 */

/** @brief Identifier of the list_all_bondings_complete event */
#define sl_bt_evt_sm_list_all_bondings_complete_id                   0x060f00a0

/** @} */ // end addtogroup sl_bt_evt_sm_list_all_bondings_complete

/***************************************************************************//**
 *
 * Configure security requirements and I/O capabilities of the system.
 *
 * @param[in] flags @parblock
 *   Security requirement bitmask.
 *
 *   Bit 0:
 *     - <b>0:</b> Allow bonding without authentication
 *     - <b>1:</b> Bonding requires authentication (Man-in-the-Middle
 *       protection)
 *
 *   Bit 1:
 *     - <b>0:</b> Allow encryption without bonding
 *     - <b>1:</b> Encryption requires bonding. Note that this setting will also
 *       enable bonding.
 *
 *   Bit 2:
 *     - <b>0:</b> Allow bonding with legacy pairing
 *     - <b>1:</b> Secure connections only
 *
 *   Bit 3:
 *     - <b>0:</b> Bonding request does not need to be confirmed
 *     - <b>1:</b> Bonding requests need to be confirmed. Received bonding
 *       requests are notified by @ref sl_bt_evt_sm_confirm_bonding
 *
 *   Bit 4:
 *     - <b>0:</b> Allow all connections
 *     - <b>1:</b> Allow connections only from bonded devices
 *
 *   Bit 5:
 *     - <b>0:</b> Prefer just works pairing when both options are possible
 *       based on the settings.
 *     - <b>1:</b> Prefer authenticated pairing when both options are possible
 *       based on the settings.
 *
 *   Bit 6 to 7: Reserved
 *
 *   Default value: 0x00
 *   @endparblock
 * @param[in] io_capabilities Enum @ref sl_bt_sm_io_capability_t. I/O
 *   Capabilities. The default I/O Capability used by the stack is No Input and
 *   No Output. Values:
 *     - <b>sl_bt_sm_io_capability_displayonly (0x0):</b> Display Only
 *     - <b>sl_bt_sm_io_capability_displayyesno (0x1):</b> Display with
 *       Yes/No-buttons
 *     - <b>sl_bt_sm_io_capability_keyboardonly (0x2):</b> Keyboard Only
 *     - <b>sl_bt_sm_io_capability_noinputnooutput (0x3):</b> No Input and No
 *       Output
 *     - <b>sl_bt_sm_io_capability_keyboarddisplay (0x4):</b> Display with
 *       Keyboard
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_configure(uint8_t flags, uint8_t io_capabilities);

/***************************************************************************//**
 *
 * Set the minimum allowed key size used for bonding. The default value is 16
 * bytes.
 *
 * @param[in] minimum_key_size Minimum allowed key size for bonding. Range: 7 to
 *   16
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_set_minimum_key_size(uint8_t minimum_key_size);

/***************************************************************************//**
 *
 * Set Security Manager in debug mode. In this mode, the secure connections
 * bonding uses known debug keys, so that the encrypted packet can be opened by
 * Bluetooth protocol analyzer. To disable the debug mode, restart the device.
 *
 * Bondings made in debug mode are unsecure.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_set_debug_mode();

/***************************************************************************//**
 *
 * Add device to accept list, which can be enabled with @ref
 * sl_bt_gap_enable_whitelisting.
 *
 * @param[in] address Address of the device added to accept list
 * @param[in] address_type Enum @ref sl_bt_gap_address_type_t. Address type of
 *   the device added to accept list. Values:
 *     - <b>sl_bt_gap_public_address (0x0):</b> Public device address
 *     - <b>sl_bt_gap_static_address (0x1):</b> Static device address
 *     - <b>sl_bt_gap_random_resolvable_address (0x2):</b> Resolvable private
 *       random address
 *     - <b>sl_bt_gap_random_nonresolvable_address (0x3):</b> Non-resolvable
 *       private random address
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_add_to_whitelist(bd_addr address, uint8_t address_type);

/***************************************************************************//**
 *
 * Set the maximum allowed bonding count and bonding policy. The maximum number
 * of bondings that can be supported depends on how much user data is stored in
 * the NVM and the NVM size. When bond policy value 1 or 2 is selected, the
 * stack will automatically write the new bond, as per the policy, only if the
 * maximum allowed bonding count has been reached. If the stack can't write a
 * new bond for any other reason (e.g., NVM is full), an error will be thrown
 * through the bonding_failed event indicating why the bonding was not written.
 * The application has to manually release space from the NVM (e.g., by deleting
 * one of the existing bonds or application data) so that a new bond can be
 * saved. The default value is 13.
 *
 * @param[in] max_bonding_count Maximum allowed bonding count. Range: 1 to 32
 * @param[in] policy_flags @parblock
 *   Bonding policy. Values:
 *     - <b>0:</b> If database is full, new bonding attempts will fail
 *     - <b>1:</b> New bonding will overwrite the oldest existing bonding
 *     - <b>2:</b> New bonding will overwrite the bonding that was used the
 *       longest time ago
 *
 *   Default: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_store_bonding_configuration(uint8_t max_bonding_count,
                                                 uint8_t policy_flags);

/***************************************************************************//**
 *
 * Set whether the device should accept new bondings. By default, the device
 * does not accept new bondings.
 *
 * @param[in] bondable @parblock
 *   Bondable mode. Values:
 *     - <b>0:</b> New bondings not accepted
 *     - <b>1:</b> Bondings allowed
 *
 *   Default value: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_set_bondable_mode(uint8_t bondable);

/***************************************************************************//**
 *
 * Enter a fixed passkey, which will be used in the @ref
 * sl_bt_evt_sm_passkey_display event.
 *
 * @param[in] passkey Passkey. Valid range: 0-999999. Set -1 to disable and
 *   start using random passkeys.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_set_passkey(int32_t passkey);

/***************************************************************************//**
 *
 * <b>Deprecated</b> and replaced by @ref sl_bt_sm_set_legacy_oob command.
 *
 * Set OOB data (out-of-band encryption data) for legacy pairing for a device.
 * OOB data may be, for example, a PIN code exchanged over an alternate path,
 * such as NFC. The device will not allow any other bonding if OOB data is set.
 * OOB data can't be set simultaneously with secure connections OOB data.
 *
 * @param[in] oob_data_len Length of data in @p oob_data
 * @param[in] oob_data OOB data. To set OOB data, send a 16-byte array. Values
 *   are in little endian format. To clear OOB data, send a zero-length array.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
SL_BGAPI_DEPRECATED sl_status_t sl_bt_sm_set_oob_data(size_t oob_data_len, const uint8_t* oob_data);

/***************************************************************************//**
 *
 * <b>Deprecated</b> and replaced by @ref sl_bt_sm_set_oob command.
 *
 * Enable the use of OOB data (out-of-band encryption data) for a device for
 * secure connections pairing. Enabling will generate new OOB data and confirm
 * values, which can be sent to the remote device. After enabling the secure
 * connections OOB data, the remote devices OOB data can be set with @ref
 * sl_bt_sm_set_sc_remote_oob_data. Calling this function will erase any set
 * remote device OOB data and confirm values. The device will not allow any
 * other bonding if OOB data is set. The secure connections OOB data cannot be
 * enabled simultaneously with legacy pairing OOB data.
 *
 * @param[in] enable Enable OOB with secure connections pairing. Values:
 *     - <b>0:</b> disable
 *     - <b>1:</b> enable
 * @param[in] max_oob_data_size Size of output buffer passed in @p oob_data
 * @param[out] oob_data_len On return, set to the length of output data written
 *   to @p oob_data
 * @param[out] oob_data OOB data. 32-byte array. The first 16-bytes contain
 *   randomly-generated OOB data and the last 16-bytes confirm value. Values are
 *   in little endian format.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
SL_BGAPI_DEPRECATED sl_status_t sl_bt_sm_use_sc_oob(uint8_t enable,
                                size_t max_oob_data_size,
                                size_t *oob_data_len,
                                uint8_t *oob_data);

/***************************************************************************//**
 *
 * <b>Deprecated</b> and replaced by @ref sl_bt_sm_set_remote_oob command.
 *
 * Set OOB data and confirm values (out-of-band encryption) received from the
 * remote device for secure connections pairing. OOB data must be enabled with
 * @ref sl_bt_sm_use_sc_oob before setting the remote device OOB data.
 *
 * @param[in] oob_data_len Length of data in @p oob_data
 * @param[in] oob_data Remote device OOB data and confirm values. To set OOB
 *   data, send a 32-byte array. First 16-bytes is OOB data and last 16-bytes
 *   the confirm value. Values are in little endian format. To clear OOB data,
 *   send a zero-length array.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
SL_BGAPI_DEPRECATED sl_status_t sl_bt_sm_set_sc_remote_oob_data(size_t oob_data_len,
                                            const uint8_t* oob_data);

/***************************************************************************//**
 *
 * Enhance the security of a connection to current security requirements. On an
 * unencrypted connection, it will encrypt the connection and will also perform
 * bonding if requested by both devices. On an encrypted connection, it will
 * cause the connection to be re-encrypted.
 *
 * @param[in] connection Connection handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_parameters - Triggered after increasing
 *     security has been completed successfully and indicates the latest
 *     security mode of the connection.
 *   - @ref sl_bt_evt_sm_bonded - Triggered if pairing or bonding was performed
 *     in this operation and the result is successful.
 *   - @ref sl_bt_evt_sm_bonding_failed - Triggered if pairing or bonding was
 *     performed in this operation and the result has failed.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_increase_security(uint8_t connection);

/***************************************************************************//**
 *
 * Enter a passkey after receiving a passkey request event.
 *
 * @param[in] connection Connection handle
 * @param[in] passkey Passkey. Valid range: 0-999999. Set -1 to cancel pairing.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_enter_passkey(uint8_t connection, int32_t passkey);

/***************************************************************************//**
 *
 * Accept or reject the reported passkey confirm value.
 *
 * @param[in] connection Connection handle
 * @param[in] confirm Acceptance. Values:
 *     - <b>0:</b> Reject
 *     - <b>1:</b> Accept confirm value
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_passkey_confirm(uint8_t connection, uint8_t confirm);

/***************************************************************************//**
 *
 * Accept or reject the bonding request.
 *
 * @param[in] connection Connection handle
 * @param[in] confirm Acceptance. Values:
 *     - <b>0:</b> Reject
 *     - <b>1:</b> Accept bonding request
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_bonding_confirm(uint8_t connection, uint8_t confirm);

/***************************************************************************//**
 *
 * <b>Deprecated</b> and replaced by @ref sl_bt_sm_get_bonding_handles and @ref
 * sl_bt_sm_get_bonding_details commands.
 *
 * List all bondings stored in the bonding database. Bondings are reported by
 * the @ref sl_bt_evt_sm_list_bonding_entry event for each bonding and the
 * report is ended with @ref sl_bt_evt_sm_list_all_bondings_complete event. Use
 * only for debugging purposes because reading from the persistent store is
 * relatively slow.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_sm_list_bonding_entry
 *   - @ref sl_bt_evt_sm_list_all_bondings_complete
 *
 ******************************************************************************/
SL_BGAPI_DEPRECATED sl_status_t sl_bt_sm_list_all_bondings();

/***************************************************************************//**
 *
 * Delete specified bonding information or accept list filtering from the
 * persistent store.
 *
 * @param[in] bonding Bonding handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_delete_bonding(uint8_t bonding);

/***************************************************************************//**
 *
 * Delete all bonding information and accept list filtering from the persistent
 * store. This will also delete device local identity resolving key (IRK).
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_delete_bondings();

/***************************************************************************//**
 *
 * Get number of entries and bitmask of their handles saved in the bonding
 * database. The entry in the bonding database can be either bonding or accept
 * list filtering device. Use @ref sl_bt_sm_get_bonding_details to find the type
 * of bonding entry and the device's address.
 *
 * @param[in] reserved Use the value 0 on this reserved field. Do not use
 *   none-zero values because they are reserved for future use.
 * @param[out] num_bondings Total number of bondings and accept list filtering
 *   devices stored in bonding database.
 * @param[in] max_bondings_size Size of output buffer passed in @p bondings
 * @param[out] bondings_len On return, set to the length of output data written
 *   to @p bondings
 * @param[out] bondings 4 byte bit field of used bonding handles in little
 *   endian format. Bit 0 of first byte is bonding handle 0, bit 0 of second
 *   byte is bonding handle 8 etc. If the bit is 1 that bonding handle exists in
 *   the bonding database.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_get_bonding_handles(uint32_t reserved,
                                         uint32_t *num_bondings,
                                         size_t max_bondings_size,
                                         size_t *bondings_len,
                                         uint8_t *bondings);

/***************************************************************************//**
 *
 * Get the detailed information for a bonding entry. Data includes remote device
 * address and address type as well as security mode for bonding and a used
 * encryption key length.
 *
 * @param[in] bonding Bonding handle
 * @param[out] address Bluetooth address of the remote device
 * @param[out] address_type Enum @ref sl_bt_gap_address_type_t. Address type.
 *   Values:
 *     - <b>sl_bt_gap_public_address (0x0):</b> Public device address
 *     - <b>sl_bt_gap_static_address (0x1):</b> Static device address
 * @param[out] security_mode Enum @ref sl_bt_connection_security_t. Connection
 *   security mode. Accept list filtering entry has security mode as no
 *   security. Values:
 *     - <b>sl_bt_connection_mode1_level1 (0x0):</b> No security
 *     - <b>sl_bt_connection_mode1_level2 (0x1):</b> Unauthenticated pairing
 *       with encryption
 *     - <b>sl_bt_connection_mode1_level3 (0x2):</b> Authenticated pairing with
 *       encryption
 *     - <b>sl_bt_connection_mode1_level4 (0x3):</b> Authenticated Secure
 *       Connections pairing with encryption using a 128-bit strength encryption
 *       key
 * @param[out] key_size Key length in bytes, 0 for accept list filtering entry
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_get_bonding_details(uint32_t bonding,
                                         bd_addr *address,
                                         uint8_t *address_type,
                                         uint8_t *security_mode,
                                         uint8_t *key_size);

/***************************************************************************//**
 *
 * Find the bonding or accept list filtering entry by using a Bluetooth device
 * address.
 *
 * @param[in] address The Bluetooth device address
 * @param[out] bonding The bonding handle
 * @param[out] security_mode Enum @ref sl_bt_connection_security_t. Connection
 *   security mode. Accept list filtering entry has security mode as no
 *   security. Values:
 *     - <b>sl_bt_connection_mode1_level1 (0x0):</b> No security
 *     - <b>sl_bt_connection_mode1_level2 (0x1):</b> Unauthenticated pairing
 *       with encryption
 *     - <b>sl_bt_connection_mode1_level3 (0x2):</b> Authenticated pairing with
 *       encryption
 *     - <b>sl_bt_connection_mode1_level4 (0x3):</b> Authenticated Secure
 *       Connections pairing with encryption using a 128-bit strength encryption
 *       key
 * @param[out] key_size Key length in bytes, 0 for accept list filtering entry
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_find_bonding_by_address(bd_addr address,
                                             uint32_t *bonding,
                                             uint8_t *security_mode,
                                             uint8_t *key_size);

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Set key for bonding or accept list filtering entry overwriting any possible
 * existing key. If there is error which leaves bonding entry in inconsistent
 * state the bonding will be deleted. Only IRK can be added to accept list
 * filtering entry.
 *
 * @param[in] bonding Bonding handle
 * @param[in] key_type Enum @ref sl_bt_sm_bonding_key_t. Key type. Values:
 *     - <b>sl_bt_sm_bonding_key_remote_ltk (0x1):</b> LTK used as central
 *       device, always used when paired using secure connections and local LTK
 *       does not exist
 *     - <b>sl_bt_sm_bonding_key_local_ltk (0x2):</b> LTK used as peripheral
 *       device when using legacy pairing, only used with secure connections
 *       pairing if key exists
 *     - <b>sl_bt_sm_bonding_key_irk (0x3):</b> Identity resolving key for
 *       resolvable private addresses
 * @param[in] key Bonding key in little endian format.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @endcond
 ******************************************************************************/
sl_status_t sl_bt_sm_set_bonding_key(uint32_t bonding,
                                     uint8_t key_type,
                                     aes_key_128 key);

/***************************************************************************//**
 *
 * Set Out-Of-Band (OOB) encryption data for a legacy pairing of a device. OOB
 * data may be, for example, a PIN code exchanged over an alternate path, such
 * as NFC. The device will not allow any other bonding if OOB data is set. OOB
 * data can't be set simultaneously with secure connections OOB data.
 *
 * @param[in] enable Enable OOB with legacy pairing. Values:
 *     - <b>0:</b> disable
 *     - <b>1:</b> enable
 * @param[in] oob_data 16-byte legacy pairing OOB data in little endian format.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_set_legacy_oob(uint8_t enable, aes_key_128 oob_data);

/***************************************************************************//**
 *
 * Enable the use of Out-Of-Band (OOB) encryption data for a device for secure
 * connections pairing. Enabling will generate new OOB data and confirm values,
 * which can be sent to the remote device. After enabling the secure connections
 * OOB data, the remote devices OOB data can be set with @ref
 * sl_bt_sm_set_remote_oob. Calling this function will erase any set remote
 * device OOB data and confirm values. The device will not allow any other
 * bonding if OOB data is set. The secure connections OOB data cannot be enabled
 * simultaneously with legacy pairing OOB data.
 *
 * @param[in] enable Enable OOB with secure connections pairing. Values:
 *     - <b>0:</b> disable
 *     - <b>1:</b> enable
 * @param[out] random 16-byte randomly-generated secure connections OOB data in
 *   little endian format.
 * @param[out] confirm 16-byte confirm value for the OOB random value in little
 *   endian format.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_set_oob(uint8_t enable,
                             aes_key_128 *random,
                             aes_key_128 *confirm);

/***************************************************************************//**
 *
 * Set Out-Of-Band (OOB) data and confirm values received from the remote device
 * for secure connections pairing. OOB data must be enabled with @ref
 * sl_bt_sm_set_oob before setting the remote device OOB data.
 *
 * @param[in] enable Enable remote device OOB data with secure connections
 *   pairing. Values:
 *     - <b>0:</b> disable
 *     - <b>1:</b> enable
 * @param[in] random 16-byte remote device secure connections OOB data in little
 *   endian format.
 * @param[in] confirm 16-byte remote device confirm value for the OOB random
 *   value in little endian format.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_set_remote_oob(uint8_t enable,
                                    aes_key_128 random,
                                    aes_key_128 confirm);

/** @} */ // end addtogroup sl_bt_sm

/**
 * @addtogroup sl_bt_ota OTA
 * @{
 *
 * @brief OTA
 *
 * Commands for configuring OTA DFU.
 */

/* Command and Response IDs */
#define sl_bt_cmd_ota_set_device_name_id                             0x01100020
#define sl_bt_cmd_ota_set_advertising_data_id                        0x02100020
#define sl_bt_cmd_ota_set_configuration_id                           0x03100020
#define sl_bt_cmd_ota_set_rf_path_id                                 0x04100020
#define sl_bt_rsp_ota_set_device_name_id                             0x01100020
#define sl_bt_rsp_ota_set_advertising_data_id                        0x02100020
#define sl_bt_rsp_ota_set_configuration_id                           0x03100020
#define sl_bt_rsp_ota_set_rf_path_id                                 0x04100020

/***************************************************************************//**
 *
 * Set the device name to be used during the OTA update. The name is stored in
 * the persistent store. Maximum name length is 17 bytes.
 *
 * Default is "OTA" if a name is not set.
 *
 * @param[in] name_len Length of data in @p name
 * @param[in] name OTA device name
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_ota_set_device_name(size_t name_len, const uint8_t* name);

/***************************************************************************//**
 *
 * Set advertising packets in OTA. Maximum 31 bytes of data can be set.
 *
 * @param[in] packet_type This value selects whether data is intended for
 *   advertising packets or scan response packets.
 *     - <b>2:</b> OTA advertising packets
 *     - <b>4:</b> OTA scan response packets
 * @param[in] adv_data_len Length of data in @p adv_data
 * @param[in] adv_data Data to be set
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_ota_set_advertising_data(uint8_t packet_type,
                                           size_t adv_data_len,
                                           const uint8_t* adv_data);

/***************************************************************************//**
 *
 * Set OTA configuration. The setting is stored in the persistent store.
 *
 * @param[in] flags @parblock
 *   OTA configuration flags. This value is given as a bitmask. Flags:
 *
 *   Bit 0: Advertising address
 *
 *     - <b>0:</b> Use public device address
 *     - <b>1:</b> Use a random address
 *
 *   Bit 1: Application update version check. Check the version number and
 *   product ID of the application upgrade before applying. If the version
 *   number of the current application can not be determined, this
 *   implementation will assume that it is OK to apply the new image. Note that
 *   this is not a security feature.
 *
 *     - <b>0:</b> Disable version check
 *     - <b>1:</b> Enable version check
 *
 *   Bit 2 to 31: Reserved
 *
 *   Default value: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_ota_set_configuration(uint32_t flags);

/***************************************************************************//**
 *
 * Set RF path antenna for OTA. Should be used only if device has multiple
 * antenna ports. The setting is stored in the persistent store.
 *
 * @param[in] enable If enabled antenna selection is used in OTA, otherwise
 *   default antenna is used.
 * @param[in] antenna Set antenna used in OTA. Value should be come from
 *   RAIL_AntennaSel_t enum. See antenna path selection in RAIL
 *   rail_chip_specific.h.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_ota_set_rf_path(uint8_t enable, uint8_t antenna);

/** @} */ // end addtogroup sl_bt_ota

/**
 * @addtogroup sl_bt_coex Coexistence
 * @{
 *
 * @brief Coexistence
 *
 * Coexistence BGAPI class. Coexistence interface is enabled and initialized
 * with sl_bt_init_coex_hal function.
 */

/* Command and Response IDs */
#define sl_bt_cmd_coex_set_options_id                                0x00200020
#define sl_bt_cmd_coex_set_parameters_id                             0x02200020
#define sl_bt_cmd_coex_set_directional_priority_pulse_id             0x03200020
#define sl_bt_cmd_coex_get_counters_id                               0x01200020
#define sl_bt_rsp_coex_set_options_id                                0x00200020
#define sl_bt_rsp_coex_set_parameters_id                             0x02200020
#define sl_bt_rsp_coex_set_directional_priority_pulse_id             0x03200020
#define sl_bt_rsp_coex_get_counters_id                               0x01200020

/**
 * @brief Coexistence configuration options
 */
typedef enum
{
  sl_bt_coex_option_enable        = 0x100, /**< (0x100) Enable coexistence
                                                feature */
  sl_bt_coex_option_tx_abort      = 0x400, /**< (0x400) Abort transmission if
                                                grant is denied */
  sl_bt_coex_option_high_priority = 0x800  /**< (0x800) Enable priority signal */
} sl_bt_coex_option_t;

/***************************************************************************//**
 *
 * Configure coexistence options at runtime.
 *
 * @param[in] mask Enum @ref sl_bt_coex_option_t. Mask defines which coexistence
 *   options are changed.
 * @param[in] options Enum @ref sl_bt_coex_option_t. Value of options to be
 *   changed. This parameter is used together with the mask parameter.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_coex_set_options(uint32_t mask, uint32_t options);

/***************************************************************************//**
 *
 * Configure coexistence parameters.
 *
 * @param[in] priority Coexistence priority threshold. Coexistence priority is
 *   toggled if priority is below this value.
 * @param[in] request Coexistence request threshold. Coexistence request is
 *   toggled if priority is below this value.
 * @param[in] pwm_period PWM functionality period length in 1 ms units
 * @param[in] pwm_dutycycle PWM functionality duty cycle in percentage
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_coex_set_parameters(uint8_t priority,
                                      uint8_t request,
                                      uint8_t pwm_period,
                                      uint8_t pwm_dutycycle);

/***************************************************************************//**
 *
 * Set Directional Priority Pulse Width
 *
 * @param[in] pulse Directional priority pulse width in us
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_coex_set_directional_priority_pulse(uint8_t pulse);

/***************************************************************************//**
 *
 * Read coexistence statistic counters from the device. Response contains the
 * list of uint32 type counter values. Counters in the list are in following
 * order: low priority requested, high priority requested, low priority denied,
 * high priority denied, low-priority TX aborted, and high-priority TX aborted.
 * Passing a non-zero value also resets counters.
 *
 * @param[in] reset Reset counters if parameter value is not zero.
 * @param[in] max_counters_size Size of output buffer passed in @p counters
 * @param[out] counters_len On return, set to the length of output data written
 *   to @p counters
 * @param[out] counters Coexistence statistic counters
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_coex_get_counters(uint8_t reset,
                                    size_t max_counters_size,
                                    size_t *counters_len,
                                    uint8_t *counters);

/** @} */ // end addtogroup sl_bt_coex

/**
 * @cond RESTRICTED
 * @addtogroup sl_bt_l2cap L2CAP
 * @{
 *
 * @brief L2CAP
 *
 * The commands and events in this class are related to the Logical Link Control
 * and Adaptation Protocol (L2CAP) in Bluetooth.
 */

/* Command and Response IDs */
#define sl_bt_cmd_l2cap_coc_send_connection_request_id               0x01430020
#define sl_bt_cmd_l2cap_coc_send_connection_response_id              0x02430020
#define sl_bt_cmd_l2cap_coc_send_le_flow_control_credit_id           0x03430020
#define sl_bt_cmd_l2cap_coc_send_disconnection_request_id            0x04430020
#define sl_bt_cmd_l2cap_coc_send_data_id                             0x05430020
#define sl_bt_rsp_l2cap_coc_send_connection_request_id               0x01430020
#define sl_bt_rsp_l2cap_coc_send_connection_response_id              0x02430020
#define sl_bt_rsp_l2cap_coc_send_le_flow_control_credit_id           0x03430020
#define sl_bt_rsp_l2cap_coc_send_disconnection_request_id            0x04430020
#define sl_bt_rsp_l2cap_coc_send_data_id                             0x05430020

/**
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 * @brief Result values for LE credit-based connection response.
 */
typedef enum
{
  sl_bt_l2cap_connection_successful            = 0x0, /**< (0x0) Connection
                                                           successful */
  sl_bt_l2cap_le_psm_not_supported             = 0x2, /**< (0x2) Connection
                                                           refused - LE_PSM not
                                                           supported */
  sl_bt_l2cap_no_resources_available           = 0x4, /**< (0x4) Connection
                                                           refused - no
                                                           resources available */
  sl_bt_l2cap_insufficient_authentication      = 0x5, /**< (0x5) Connection
                                                           refused -
                                                           insufficient
                                                           authentication */
  sl_bt_l2cap_insufficient_authorization       = 0x6, /**< (0x6) Connection
                                                           refused -
                                                           insufficient
                                                           authorization */
  sl_bt_l2cap_insufficient_encryption_key_size = 0x7, /**< (0x7) Connection
                                                           refused -
                                                           insufficient
                                                           encryption key size */
  sl_bt_l2cap_insufficient_encryption          = 0x8, /**< (0x8) Connection
                                                           refused -
                                                           insufficient
                                                           encryption */
  sl_bt_l2cap_invalid_source_cid               = 0x9, /**< (0x9) Connection
                                                           refused - invalid
                                                           Source CID */
  sl_bt_l2cap_source_cid_already_allocated     = 0xa, /**< (0xa) Connection
                                                           refused - Source CID
                                                           already allocated */
  sl_bt_l2cap_unacceptable_parameters          = 0xb  /**< (0xb) Connection
                                                           refused -
                                                           unacceptable
                                                           parameters */
} sl_bt_l2cap_coc_connection_result_t;

/**
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 * @brief Reason codes describe why the request packet was rejected.
 */
typedef enum
{
  sl_bt_l2cap_command_not_understood = 0x0, /**< (0x0) Command not understood
                                                 e.g., unknown command code */
  sl_bt_l2cap_signaling_mtu_exceeded = 0x1, /**< (0x1) Command size has exceeded
                                                 signaling MTU size */
  sl_bt_l2cap_invalid_cid_request    = 0x2  /**< (0x2) An invalid CID is
                                                 included in the command */
} sl_bt_l2cap_command_reject_reason_t;

/**
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 * @brief Command codes describe which of the request commands has been rejected.
 */
typedef enum
{
  sl_bt_l2cap_disconnection_request = 0x6,  /**< (0x6) Disconnection request */
  sl_bt_l2cap_connection_request    = 0x14, /**< (0x14) LE credit-based
                                                 connection request */
  sl_bt_l2cap_flow_control_credit   = 0x16  /**< (0x16) LE Flow Control Credit */
} sl_bt_l2cap_command_code_t;

/**
 * @addtogroup sl_bt_evt_l2cap_coc_connection_request sl_bt_evt_l2cap_coc_connection_request
 * @{
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Indicates that an LE credit-based connection request is received
 *
 * The application will respond with @ref
 * sl_bt_l2cap_coc_send_connection_response command.
 */

/** @brief Identifier of the coc_connection_request event */
#define sl_bt_evt_l2cap_coc_connection_request_id                    0x014300a0

/***************************************************************************//**
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Data structure of the coc_connection_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_l2cap_coc_connection_request_s
{
  uint8_t  connection;          /**< Handle of the LE connection which opening
                                     of connection-oriented channel is requested */
  uint16_t le_psm;              /**< LE Protocol/Service Multiplexer - LE_PSM */
  uint16_t source_cid;          /**< The source CID represents a channel
                                     endpoint on the device sending the request */
  uint16_t mtu;                 /**< The MTU size of the device sending the
                                     request. */
  uint16_t mps;                 /**< The MPS size of the device sending the
                                     request. */
  uint16_t initial_credit;      /**< The initial credit value of the device
                                     sending the request */
  uint8_t  flags;               /**< This field indicates the security mode of
                                     the LE connection to let applications
                                     evaluate security requirements.
                                       - <b>Bit 0:</b> Encrypted
                                       - <b>Bit 1:</b> Authenticated
                                       - <b>Bit 2:</b> Authorized

                                     Remaining bits are reserved for future use. */
  uint8_t  encryption_key_size; /**< Key size used by the link */
});

typedef struct sl_bt_evt_l2cap_coc_connection_request_s sl_bt_evt_l2cap_coc_connection_request_t;

/** @} */ // end addtogroup sl_bt_evt_l2cap_coc_connection_request

/**
 * @addtogroup sl_bt_evt_l2cap_coc_connection_response sl_bt_evt_l2cap_coc_connection_response
 * @{
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Indicates that an LE credit-based connection response is received
 *
 * Ignore other fields if the result field indicates that the connection was
 * refused.
 */

/** @brief Identifier of the coc_connection_response event */
#define sl_bt_evt_l2cap_coc_connection_response_id                   0x024300a0

/***************************************************************************//**
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Data structure of the coc_connection_response event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_l2cap_coc_connection_response_s
{
  uint8_t  connection;      /**< Handle of the LE connection which response
                                 message is received */
  uint16_t destination_cid; /**< The destination CID represents a channel
                                 endpoint on the device sending the response,
                                 which will become destination channel endpoint
                                 of the device receiving the event */
  uint16_t mtu;             /**< The MTU size of the device sending the response */
  uint16_t mps;             /**< The MPS size of the device sending the request */
  uint16_t initial_credit;  /**< The initial credit value of the device sending
                                 the request. */
  uint16_t l2cap_errorcode; /**< Enum @ref sl_bt_l2cap_coc_connection_result_t.
                                 The l2cap_errorcode field indicates the result
                                 of the connection request. */
});

typedef struct sl_bt_evt_l2cap_coc_connection_response_s sl_bt_evt_l2cap_coc_connection_response_t;

/** @} */ // end addtogroup sl_bt_evt_l2cap_coc_connection_response

/**
 * @addtogroup sl_bt_evt_l2cap_coc_le_flow_control_credit sl_bt_evt_l2cap_coc_le_flow_control_credit
 * @{
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Indicates that a LE flow control credit is received so the channel
 * endpoint on remote device is capable of receiving more data
 */

/** @brief Identifier of the coc_le_flow_control_credit event */
#define sl_bt_evt_l2cap_coc_le_flow_control_credit_id                0x034300a0

/***************************************************************************//**
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Data structure of the coc_le_flow_control_credit event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_l2cap_coc_le_flow_control_credit_s
{
  uint8_t  connection; /**< Handle of the LE connection which LE flow control
                            credit message is received */
  uint16_t cid;        /**< The CID represents the destination channel endpoint
                            of the device receiving the flow control credit. */
  uint16_t credits;    /**< The credit value indicates the additional number of
                            PDUs that the device can send. */
});

typedef struct sl_bt_evt_l2cap_coc_le_flow_control_credit_s sl_bt_evt_l2cap_coc_le_flow_control_credit_t;

/** @} */ // end addtogroup sl_bt_evt_l2cap_coc_le_flow_control_credit

/**
 * @addtogroup sl_bt_evt_l2cap_coc_channel_disconnected sl_bt_evt_l2cap_coc_channel_disconnected
 * @{
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Indicates that a L2CAP connection-oriented channel is disconnected.
 */

/** @brief Identifier of the coc_channel_disconnected event */
#define sl_bt_evt_l2cap_coc_channel_disconnected_id                  0x044300a0

/***************************************************************************//**
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Data structure of the coc_channel_disconnected event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_l2cap_coc_channel_disconnected_s
{
  uint8_t  connection; /**< Handle of the LE connection, which the
                            connection-oriented channel is disconnected */
  uint16_t cid;        /**< The CID represents the destination channel endpoint
                            of the device receiving the event. */
  uint16_t reason;     /**< The reason for channel disconnection */
});

typedef struct sl_bt_evt_l2cap_coc_channel_disconnected_s sl_bt_evt_l2cap_coc_channel_disconnected_t;

/** @} */ // end addtogroup sl_bt_evt_l2cap_coc_channel_disconnected

/**
 * @addtogroup sl_bt_evt_l2cap_coc_data sl_bt_evt_l2cap_coc_data
 * @{
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Indicates that there is data received on a L2CAP connection-oriented
 * channel.
 */

/** @brief Identifier of the coc_data event */
#define sl_bt_evt_l2cap_coc_data_id                                  0x054300a0

/***************************************************************************//**
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Data structure of the coc_data event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_l2cap_coc_data_s
{
  uint8_t    connection; /**< Handle of the LE connection which data is received */
  uint16_t   cid;        /**< The CID represents the source channel endpoint on
                              the device sending data. */
  uint8array data;       /**< Data received. */
});

typedef struct sl_bt_evt_l2cap_coc_data_s sl_bt_evt_l2cap_coc_data_t;

/** @} */ // end addtogroup sl_bt_evt_l2cap_coc_data

/**
 * @addtogroup sl_bt_evt_l2cap_command_rejected sl_bt_evt_l2cap_command_rejected
 * @{
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Indicates that the remote device received the command and it was
 * rejected
 *
 * This might happen if, for example, the remote does not support the feature or
 * the incorrect CID is in the command that was sent.
 */

/** @brief Identifier of the command_rejected event */
#define sl_bt_evt_l2cap_command_rejected_id                          0x064300a0

/***************************************************************************//**
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Data structure of the command_rejected event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_l2cap_command_rejected_s
{
  uint8_t  connection; /**< Handle of the LE connection which event is received */
  uint8_t  code;       /**< Enum @ref sl_bt_l2cap_command_code_t. Code of the
                            command has been rejected by the sender of the
                            event. */
  uint16_t reason;     /**< Enum @ref sl_bt_l2cap_command_reject_reason_t.
                            Reason field describe why the L2CAP command was
                            rejected. */
});

typedef struct sl_bt_evt_l2cap_command_rejected_s sl_bt_evt_l2cap_command_rejected_t;

/** @} */ // end addtogroup sl_bt_evt_l2cap_command_rejected

/***************************************************************************//**
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Send LE credit-based connection request.
 *
 * @param[in] connection Handle of the LE connection to be used for opening
 *   connection-oriented channel.
 * @param[in] le_psm LE Protocol/Service Multiplexer - LE_PSM
 * @param[in] mtu @parblock
 *   The maximum size of payload data that the application on the device sending
 *   the request can accept, i.e., the MTU corresponds to the maximum SDU size.
 *
 *   Range: 23 to 65533.
 *
 *   Application needs to handle segmentation and reassembly from PDU to SDU.
 *   @endparblock
 * @param[in] mps @parblock
 *   The maximum size of payload data that the L2CAP layer on the device sending
 *   the request can accept, i.e., the MPS corresponds to the maximum PDU
 *   payload size.
 *
 *   Range: 23 to 250.
 *
 *   That is the maximum size of data that the application can send using @ref
 *   sl_bt_l2cap_coc_send_data command or receive by @ref
 *   sl_bt_evt_l2cap_coc_data event.
 *   @endparblock
 * @param[in] initial_credit The initial credit value indicates the number of
 *   PDUs that the peer device can send.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_l2cap_coc_connection_response - Triggered when a LE
 *     credit-based connection connection response has been received in response
 *     to this command.
 *   - @ref sl_bt_evt_l2cap_coc_channel_disconnected - Triggered when a LE
 *     credit-based connection connection response has not been received within
 *     the 30 seconds timeout in response to this command.
 *
 ******************************************************************************/
sl_status_t sl_bt_l2cap_coc_send_connection_request(uint8_t connection,
                                                    uint16_t le_psm,
                                                    uint16_t mtu,
                                                    uint16_t mps,
                                                    uint16_t initial_credit);

/***************************************************************************//**
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Send LE credit-based connection response.
 *
 * @param[in] connection Handle of the LE connection to be used for opening
 *   connection-oriented channel
 * @param[in] cid The CID represents the destination channel endpoint of the
 *   device sending the response which is same as source CID field of
 *   corresponding request message
 * @param[in] mtu @parblock
 *   The maximum size of payload data that the application on the device sending
 *   the response can accept, i.e., the MTU corresponds to the maximum SDU size.
 *
 *   Range: 23 to 65533.
 *
 *   Application needs to handle segmentation and reassembly from PDU to SDU.
 *   @endparblock
 * @param[in] mps @parblock
 *   The maximum size of payload data that the L2CAP layer on the device sending
 *   the response can accept, i.e., the MPS corresponds to the maximum PDU
 *   payload size.
 *
 *   Range: 23 to 250.
 *
 *   That is the maximum size of data that the application is able to send using
 *   @ref sl_bt_l2cap_coc_send_data command or receive by @ref
 *   sl_bt_evt_l2cap_coc_data event.
 *   @endparblock
 * @param[in] initial_credit The initial credit value indicates the number of
 *   PDUs that the peer device can send
 * @param[in] l2cap_errorcode Enum @ref sl_bt_l2cap_coc_connection_result_t. The
 *   l2cap_errorcode field indicates the result of the connection request.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_l2cap_coc_send_connection_response(uint8_t connection,
                                                     uint16_t cid,
                                                     uint16_t mtu,
                                                     uint16_t mps,
                                                     uint16_t initial_credit,
                                                     uint16_t l2cap_errorcode);

/***************************************************************************//**
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Send LE flow control credit indicating that the channel endpoint on local
 * device is capable of receiving more data.
 *
 * @param[in] connection Handle of the LE connection for sending flow control
 *   credit.
 * @param[in] cid The CID represents the destination channel endpoint of the
 *   device sending the flow control credit.
 * @param[in] credits The credit value indicates the additional number of PDUs
 *   that the peer device can send.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_l2cap_coc_send_le_flow_control_credit(uint8_t connection,
                                                        uint16_t cid,
                                                        uint16_t credits);

/***************************************************************************//**
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Send L2CAP connection-oriented channel disconnection request.
 *
 * @param[in] connection Handle of the LE connection for terminating the
 *   connection-oriented channel
 * @param[in] cid The CID represents the destination channel endpoint of the
 *   device sending the disconnection request.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_l2cap_coc_channel_disconnected - Triggered when a L2CAP
 *     channel is disconnected in response to this command.
 *
 ******************************************************************************/
sl_status_t sl_bt_l2cap_coc_send_disconnection_request(uint8_t connection,
                                                       uint16_t cid);

/***************************************************************************//**
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Send data to a L2CAP LE connection-oriented channel.
 *
 * @param[in] connection Handle of the LE connection for sending data
 * @param[in] cid The CID represents the destination channel endpoint of the
 *   device sending data.
 * @param[in] data_len Length of data in @p data
 * @param[in] data Data to be sent. Data length must be within the range of
 *   destination channel endpoint's MPS value.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_l2cap_coc_send_data(uint8_t connection,
                                      uint16_t cid,
                                      size_t data_len,
                                      const uint8_t* data);

/** @} */ // end addtogroup sl_bt_l2cap
/** @endcond */ // end restricted class

/**
 * @addtogroup sl_bt_cte_transmitter CTE Transmitter
 * @{
 *
 * @brief CTE Transmitter
 *
 * Commands and events in this class manage Constant Tone Extension (CTE)
 * transmission.
 *
 * CTE feature is only supported by specific devices. Commands from this class
 * will return SL_STATUS_NOT_SUPPORTED on devices that do not support CTE.
 */

/* Command and Response IDs */
#define sl_bt_cmd_cte_transmitter_set_dtm_parameters_id              0x04440020
#define sl_bt_cmd_cte_transmitter_clear_dtm_parameters_id            0x05440020
#define sl_bt_cmd_cte_transmitter_enable_connection_cte_id           0x00440020
#define sl_bt_cmd_cte_transmitter_disable_connection_cte_id          0x01440020
#define sl_bt_cmd_cte_transmitter_enable_connectionless_cte_id       0x02440020
#define sl_bt_cmd_cte_transmitter_disable_connectionless_cte_id      0x03440020
#define sl_bt_cmd_cte_transmitter_enable_silabs_cte_id               0x06440020
#define sl_bt_cmd_cte_transmitter_disable_silabs_cte_id              0x07440020
#define sl_bt_rsp_cte_transmitter_set_dtm_parameters_id              0x04440020
#define sl_bt_rsp_cte_transmitter_clear_dtm_parameters_id            0x05440020
#define sl_bt_rsp_cte_transmitter_enable_connection_cte_id           0x00440020
#define sl_bt_rsp_cte_transmitter_disable_connection_cte_id          0x01440020
#define sl_bt_rsp_cte_transmitter_enable_connectionless_cte_id       0x02440020
#define sl_bt_rsp_cte_transmitter_disable_connectionless_cte_id      0x03440020
#define sl_bt_rsp_cte_transmitter_enable_silabs_cte_id               0x06440020
#define sl_bt_rsp_cte_transmitter_disable_silabs_cte_id              0x07440020

/***************************************************************************//**
 *
 * Set the CTE-related parameters of the LE transmitter test.
 *
 * @param[in] cte_length @parblock
 *   The length of the Constant Tone Extension in 8 us units
 *     - <b>0:</b> No CTE
 *     - <b>0x02 to 0x14:</b> CTE length
 *
 *   Default: 0 (no CTE)
 *   @endparblock
 * @param[in] cte_type @parblock
 *   CTE type
 *     - <b>0:</b> AoA CTE
 *     - <b>1:</b> AoD CTE with 1 us slots
 *     - <b>2:</b> AoD CTE with 2 us slots
 *
 *   Default: 0
 *   @endparblock
 * @param[in] switching_pattern_len Length of data in @p switching_pattern
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over. This can be
 *   an empty array if CTE is unused.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_set_dtm_parameters(uint8_t cte_length,
                                                     uint8_t cte_type,
                                                     size_t switching_pattern_len,
                                                     const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Clear CTE-related parameters that were previously set for LE transmitter
 * test. Default values will be restored for these parameters.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_clear_dtm_parameters();

/***************************************************************************//**
 *
 * Enable different types of CTE responses on a connection. CTE response will be
 * sent once requested by the peer device using the CTE Request procedure.
 *
 * @param[in] connection Connection handle
 * @param[in] cte_types CTE types. Bitmask of the following:
 *     - <b>Bit 0:</b> AoA CTE response
 *     - <b>Bit 1:</b> AoD CTE response with 1 us slots
 *     - <b>Bit 2:</b> AoD CTE response with 2 us slots
 * @param[in] switching_pattern_len Length of data in @p switching_pattern
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_enable_connection_cte(uint8_t connection,
                                                        uint8_t cte_types,
                                                        size_t switching_pattern_len,
                                                        const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Disable CTE responses on a connection.
 *
 * @param[in] connection Connection handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_disable_connection_cte(uint8_t connection);

/***************************************************************************//**
 *
 * Start connectionless CTE transmit. CTEs will be transmitted in periodic
 * advertisement packets. As a result, a periodic advertising has to be started
 * prior to this command.
 *
 * @param[in] handle Periodic advertising handle
 * @param[in] cte_length CTE length in 8 us units.
 *     - Range: 0x02 to 0x14
 *     - Time Range: 16 us to 160 us
 * @param[in] cte_type CTE type
 *     - <b>0:</b> AoA CTE
 *     - <b>1:</b> AoD CTE with 1 us slots
 *     - <b>2:</b> AoD CTE with 2 us slots
 * @param[in] cte_count The number of CTEs to be transmitted in each periodic
 *   advertising interval
 * @param[in] switching_pattern_len Length of data in @p switching_pattern
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_enable_connectionless_cte(uint8_t handle,
                                                            uint8_t cte_length,
                                                            uint8_t cte_type,
                                                            uint8_t cte_count,
                                                            size_t switching_pattern_len,
                                                            const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Stop the connectionless CTE transmit.
 *
 * @param[in] handle Periodic advertising handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_disable_connectionless_cte(uint8_t handle);

/***************************************************************************//**
 *
 * Enable Silicon Labs CTE transmit. CTEs will be transmitted in extended
 * advertisement packets. As a result, extended advertising has to be started
 * prior this command.
 *
 * @param[in] handle Advertising handle
 * @param[in] cte_length CTE length in 8 us units.
 *     - Range: 0x02 to 0x14
 *     - Time Range: 16 us to 160 us
 * @param[in] cte_type CTE type
 *     - <b>0:</b> AoA CTE
 *     - <b>1:</b> AoD CTE with 1 us slots
 *     - <b>2:</b> AoD CTE with 2 us slots
 * @param[in] cte_count The number of CTEs to be transmitted in each extended
 *   advertising interval. Currently only cte_count = 1 is supported.
 * @param[in] switching_pattern_len Length of data in @p switching_pattern
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_enable_silabs_cte(uint8_t handle,
                                                    uint8_t cte_length,
                                                    uint8_t cte_type,
                                                    uint8_t cte_count,
                                                    size_t switching_pattern_len,
                                                    const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Disable Silicon Labs CTE transmit.
 *
 * @param[in] handle Advertising handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_disable_silabs_cte(uint8_t handle);

/** @} */ // end addtogroup sl_bt_cte_transmitter

/**
 * @addtogroup sl_bt_cte_receiver CTE Receiver
 * @{
 *
 * @brief CTE Receiver
 *
 * Commands and events in this class manage Constant Tone Extension (CTE)
 * receiving.
 *
 * CTE feature is only supported by specific devices. Commands from this class
 * will return SL_STATUS_NOT_SUPPORTED on devices that do not support CTE.
 */

/* Command and Response IDs */
#define sl_bt_cmd_cte_receiver_set_dtm_parameters_id                 0x05450020
#define sl_bt_cmd_cte_receiver_clear_dtm_parameters_id               0x06450020
#define sl_bt_cmd_cte_receiver_set_sync_cte_type_id                  0x09450020
#define sl_bt_cmd_cte_receiver_configure_id                          0x00450020
#define sl_bt_cmd_cte_receiver_enable_connection_cte_id              0x01450020
#define sl_bt_cmd_cte_receiver_disable_connection_cte_id             0x02450020
#define sl_bt_cmd_cte_receiver_enable_connectionless_cte_id          0x03450020
#define sl_bt_cmd_cte_receiver_disable_connectionless_cte_id         0x04450020
#define sl_bt_cmd_cte_receiver_enable_silabs_cte_id                  0x07450020
#define sl_bt_cmd_cte_receiver_disable_silabs_cte_id                 0x08450020
#define sl_bt_rsp_cte_receiver_set_dtm_parameters_id                 0x05450020
#define sl_bt_rsp_cte_receiver_clear_dtm_parameters_id               0x06450020
#define sl_bt_rsp_cte_receiver_set_sync_cte_type_id                  0x09450020
#define sl_bt_rsp_cte_receiver_configure_id                          0x00450020
#define sl_bt_rsp_cte_receiver_enable_connection_cte_id              0x01450020
#define sl_bt_rsp_cte_receiver_disable_connection_cte_id             0x02450020
#define sl_bt_rsp_cte_receiver_enable_connectionless_cte_id          0x03450020
#define sl_bt_rsp_cte_receiver_disable_connectionless_cte_id         0x04450020
#define sl_bt_rsp_cte_receiver_enable_silabs_cte_id                  0x07450020
#define sl_bt_rsp_cte_receiver_disable_silabs_cte_id                 0x08450020

/**
 * @addtogroup sl_bt_evt_cte_receiver_dtm_iq_report sl_bt_evt_cte_receiver_dtm_iq_report
 * @{
 * @brief IQ sample report from DTM CTE packets.
 */

/** @brief Identifier of the dtm_iq_report event */
#define sl_bt_evt_cte_receiver_dtm_iq_report_id                      0x024500a0

/***************************************************************************//**
 * @brief Data structure of the dtm_iq_report event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_cte_receiver_dtm_iq_report_s
{
  uint16_t   status;          /**< Status of CTE IQ sampling */
  uint8_t    channel;         /**< The channel on which the CTE packet was
                                   received */
  int8_t     rssi;            /**< RSSI in the received CTE packet. Unit: dBm */
  uint8_t    rssi_antenna_id; /**< The ID of the antenna on which RSSI was
                                   measured */
  uint8_t    cte_type;        /**< The CTE type
                                     - <b>0:</b> AoA CTE response
                                     - <b>1:</b> AoD CTE response with 1us slots
                                     - <b>2:</b> AoD CTE response with 2us slots */
  uint8_t    slot_durations;  /**< Slot durations
                                     - <b>1:</b> Switching and sampling slots
                                       are 1 us each
                                     - <b>2:</b> Switching and sampling slots
                                       are 2 us each */
  uint16_t   event_counter;   /**< The event counter of the periodic advertising
                                   train or the connection */
  uint8array samples;         /**< IQ samples of the received CTE packet. I and
                                   Q samples follow each other alternately (I,
                                   Q, I, Q, ...) */
});

typedef struct sl_bt_evt_cte_receiver_dtm_iq_report_s sl_bt_evt_cte_receiver_dtm_iq_report_t;

/** @} */ // end addtogroup sl_bt_evt_cte_receiver_dtm_iq_report

/**
 * @addtogroup sl_bt_evt_cte_receiver_connection_iq_report sl_bt_evt_cte_receiver_connection_iq_report
 * @{
 * @brief IQ sample report from connection CTE packets.
 */

/** @brief Identifier of the connection_iq_report event */
#define sl_bt_evt_cte_receiver_connection_iq_report_id               0x004500a0

/***************************************************************************//**
 * @brief Data structure of the connection_iq_report event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_cte_receiver_connection_iq_report_s
{
  uint16_t   status;          /**< Status of CTE IQ sampling */
  uint8_t    connection;      /**< Connection handle or periodic advertising
                                   synchronization handle */
  uint8_t    phy;             /**< The PHY on which the packet is received.
                                     - <b>1:</b> 1M PHY
                                     - <b>2:</b> 2M PHY */
  uint8_t    channel;         /**< The channel on which the CTE packet was
                                   received */
  int8_t     rssi;            /**< RSSI in the received CTE packet. Unit: dBm */
  uint8_t    rssi_antenna_id; /**< The ID of the antenna on which RSSI was
                                   measured */
  uint8_t    cte_type;        /**< The CTE type
                                     - <b>0:</b> AoA CTE response
                                     - <b>1:</b> AoD CTE response with 1us slots
                                     - <b>2:</b> AoD CTE response with 2us slots */
  uint8_t    slot_durations;  /**< Slot durations
                                     - <b>1:</b> Switching and sampling slots
                                       are 1 us each
                                     - <b>2:</b> Switching and sampling slots
                                       are 2 us each */
  uint16_t   event_counter;   /**< The event counter of the connection */
  uint8array samples;         /**< IQ samples of the received CTE packet. I and
                                   Q samples follow each other alternately (I,
                                   Q, I, Q, ...) */
});

typedef struct sl_bt_evt_cte_receiver_connection_iq_report_s sl_bt_evt_cte_receiver_connection_iq_report_t;

/** @} */ // end addtogroup sl_bt_evt_cte_receiver_connection_iq_report

/**
 * @addtogroup sl_bt_evt_cte_receiver_connectionless_iq_report sl_bt_evt_cte_receiver_connectionless_iq_report
 * @{
 * @brief IQ sample report from connectionless CTE packets.
 */

/** @brief Identifier of the connectionless_iq_report event */
#define sl_bt_evt_cte_receiver_connectionless_iq_report_id           0x014500a0

/***************************************************************************//**
 * @brief Data structure of the connectionless_iq_report event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_cte_receiver_connectionless_iq_report_s
{
  uint16_t   status;          /**< Status of CTE IQ sampling */
  uint16_t   sync;            /**< Periodic advertising synchronization handle */
  uint8_t    channel;         /**< The channel on which the CTE packet was
                                   received */
  int8_t     rssi;            /**< RSSI in the received CTE packet. Unit: dBm */
  uint8_t    rssi_antenna_id; /**< The ID of the antenna on which RSSI was
                                   measured */
  uint8_t    cte_type;        /**< The CTE type
                                     - <b>0:</b> AoA CTE response
                                     - <b>1:</b> AoD CTE response with 1us slots
                                     - <b>2:</b> AoD CTE response with 2us slots */
  uint8_t    slot_durations;  /**< Slot durations
                                     - <b>1:</b> Switching and sampling slots
                                       are 1 us each
                                     - <b>2:</b> Switching and sampling slots
                                       are 2 us each */
  uint16_t   event_counter;   /**< The event counter of the periodic advertising
                                   train */
  uint8array samples;         /**< IQ samples of the received CTE packet. I and
                                   Q samples follow each other alternately (I,
                                   Q, I, Q, ...) */
});

typedef struct sl_bt_evt_cte_receiver_connectionless_iq_report_s sl_bt_evt_cte_receiver_connectionless_iq_report_t;

/** @} */ // end addtogroup sl_bt_evt_cte_receiver_connectionless_iq_report

/**
 * @addtogroup sl_bt_evt_cte_receiver_silabs_iq_report sl_bt_evt_cte_receiver_silabs_iq_report
 * @{
 * @brief IQ samples report from Silicon Labs CTE packets.
 */

/** @brief Identifier of the silabs_iq_report event */
#define sl_bt_evt_cte_receiver_silabs_iq_report_id                   0x034500a0

/***************************************************************************//**
 * @brief Data structure of the silabs_iq_report event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_cte_receiver_silabs_iq_report_s
{
  uint16_t   status;          /**< Status of CTE IQ sampling */
  bd_addr    address;         /**< Bluetooth address of the remote device */
  uint8_t    address_type;    /**< Advertiser address type. Values:
                                     - <b>0:</b> Public address
                                     - <b>1:</b> Random address
                                     - <b>255:</b> No address provided
                                       (anonymous advertising) */
  uint8_t    phy;             /**< The PHY on which the packet is received.
                                     - <b>1:</b> 1M PHY
                                     - <b>2:</b> 2M PHY */
  uint8_t    channel;         /**< The channel on which the CTE packet was
                                   received */
  int8_t     rssi;            /**< RSSI in the received CTE packet. Unit: dBm */
  uint8_t    rssi_antenna_id; /**< The ID of the antenna on which RSSI was
                                   measured */
  uint8_t    cte_type;        /**< The CTE type
                                     - <b>0:</b> AoA CTE response
                                     - <b>1:</b> AoD CTE response with 1us slots
                                     - <b>2:</b> AoD CTE response with 2us slots */
  uint8_t    slot_durations;  /**< Slot durations
                                     - <b>1:</b> Switching and sampling slots
                                       are 1 us each
                                     - <b>2:</b> Switching and sampling slots
                                       are 2 us each */
  uint16_t   packet_counter;  /**< The event counter of the periodic advertising
                                   train or the connection */
  uint8array samples;         /**< IQ samples of the received CTE packet. I and
                                   Q samples follow each other alternately (I,
                                   Q, I, Q, ...) */
});

typedef struct sl_bt_evt_cte_receiver_silabs_iq_report_s sl_bt_evt_cte_receiver_silabs_iq_report_t;

/** @} */ // end addtogroup sl_bt_evt_cte_receiver_silabs_iq_report

/***************************************************************************//**
 *
 * Set CTE-related parameters of LE receiver test.
 *
 * @param[in] cte_length @parblock
 *   Expected CTE length in 8 us units
 *     - <b>0:</b> No CTE
 *     - <b>0x02 to 0x14:</b> Expected CTE length
 *
 *   Default: 0 (no CTE)
 *   @endparblock
 * @param[in] cte_type @parblock
 *   Expected CTE type
 *     - <b>0:</b> Expect AoA CTE
 *     - <b>1:</b> Expect AoD CTE with 1 us slots
 *     - <b>2:</b> Expect AoD CTE with 2 us slots
 *
 *   Default: 0
 *   @endparblock
 * @param[in] slot_durations @parblock
 *   Slot durations
 *     - <b>1:</b> Switching and sampling slots are 1 us each
 *     - <b>2:</b> Switching and sampling slots are 2 us each
 *
 *   Default: 1
 *   @endparblock
 * @param[in] switching_pattern_len Length of data in @p switching_pattern
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over. This can be
 *   an empty array if CTE is unused.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_cte_receiver_dtm_iq_report - Triggered when IQ samples
 *     have been received.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_set_dtm_parameters(uint8_t cte_length,
                                                  uint8_t cte_type,
                                                  uint8_t slot_durations,
                                                  size_t switching_pattern_len,
                                                  const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Clear CTE-related parameters that were previously set for LE receiver test.
 * Default values will be restored for these parameters.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_clear_dtm_parameters();

/***************************************************************************//**
 *
 * Set the sync CTE type to limit what types of periodic advertisers to sync to.
 * The set parameter takes effect immediately for all advertisers that have not
 * already established synchronization.
 *
 * @param[in] sync_cte_type @parblock
 *   Sync CTE type flags. This value can be a bitmask of multiple flags. Flags:
 *     - <b>1 (bit 0):</b> Do not sync to packets with an AoA Constant Tone
 *       Extension
 *     - <b>2 (bit 1):</b> Do not sync to packets with an AoD Constant Tone
 *       Extension with 1 us slots
 *     - <b>4 (bit 2):</b> Do not sync to packets with an AoD Constant Tone
 *       Extension with 2 us slots
 *     - <b>8 (bit 3):</b> Bit not used
 *     - <b>16 (bit 4):</b> Do not sync to packets without a Constant Tone
 *       Extension
 *
 *   Default: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_set_sync_cte_type(uint8_t sync_cte_type);

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Configure the CTE sampling mode.
 *
 * @param[in] flags @parblock
 *   Values:
 *     - <b>0:</b> Disable raw sample mode, only picked IQ samples are reported
 *       (1 IQ sample pair / slot)
 *     - <b>1:</b> Enable raw sample mode, every IQ sample is reported.
 *
 *   Default: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @endcond
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_configure(uint8_t flags);

/***************************************************************************//**
 *
 * Start IQ samplings on a connection. A CTE requests will be initiated
 * periodically on the given connection and IQ sampling will be made on the
 * received CTE responses.
 *
 * @param[in] connection Connection handle
 * @param[in] interval Measurement interval. CTE requests may be sent less
 *   often. For example, if a connetion event is missed for some reason, the CTE
 *   request will be sent in the next connection event.
 *     - <b>0:</b> No interval. The request is initiated only once.
 *     - <b>Other values N:</b> Initiate the request every N-th connection
 *       events
 * @param[in] cte_length Minimum CTE length requested in 8 us units.
 *     - Range: 0x02 to 0x14
 *     - Time Range: 16 us to 160 us
 * @param[in] cte_type Requested CTE type
 *     - <b>0:</b> AoA CTE
 *     - <b>1:</b> AoD CTE with 1 us slots
 *     - <b>2:</b> AoD CTE with 2 us slots
 * @param[in] slot_durations Slot durations
 *     - <b>1:</b> Switching and sampling slots are 1 us each
 *     - <b>2:</b> Switching and sampling slots are 2 us each
 * @param[in] switching_pattern_len Length of data in @p switching_pattern
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_cte_receiver_connection_iq_report - Triggered when IQ
 *     samples have been received.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_enable_connection_cte(uint8_t connection,
                                                     uint16_t interval,
                                                     uint8_t cte_length,
                                                     uint8_t cte_type,
                                                     uint8_t slot_durations,
                                                     size_t switching_pattern_len,
                                                     const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Stop the IQ sampling on a connection. CTEs will not be requested on the given
 * connection.
 *
 * @param[in] connection Connection handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_disable_connection_cte(uint8_t connection);

/***************************************************************************//**
 *
 * Start IQ sampling on a periodic advertising synchronization. IQ samples are
 * taken on each CTE found in the periodic advertisements.
 *
 * @param[in] sync Periodic advertising synchronization handle
 * @param[in] slot_durations Slot durations
 *     - <b>1:</b> Switching and sampling slots are 1 us each
 *     - <b>2:</b> Switching and sampling slots are 2 us each
 * @param[in] cte_count   - <b>0:</b> Sample and report all available CTEs
 *     - <b>Other values:</b> Maximum number of sampled CTEs in each periodic
 *       advertising interval
 * @param[in] switching_pattern_len Length of data in @p switching_pattern
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_cte_receiver_connectionless_iq_report - Triggered when IQ
 *     samples have been received.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_enable_connectionless_cte(uint16_t sync,
                                                         uint8_t slot_durations,
                                                         uint8_t cte_count,
                                                         size_t switching_pattern_len,
                                                         const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Stop IQ sampling on a periodic advertising synchronization.
 *
 * @param[in] sync Periodic advertising synchronization handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_disable_connectionless_cte(uint16_t sync);

/***************************************************************************//**
 *
 * Enable IQ sampling of Silicon Labs CTE found in extended advertisements.
 *
 * @param[in] slot_durations Slot durations
 *     - <b>1:</b> Switching and sampling slots are 1 us each
 *     - <b>2:</b> Switching and sampling slots are 2 us each
 * @param[in] cte_count   - <b>0:</b> Sample and report all available CTEs
 *     - <b>Other values:</b> Maximum number of sampled CTEs in each extended
 *       advertising interval
 * @param[in] switching_pattern_len Length of data in @p switching_pattern
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_cte_receiver_silabs_iq_report - Triggered when IQ samples
 *     of Silicon Labs CTE have been received.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_enable_silabs_cte(uint8_t slot_durations,
                                                 uint8_t cte_count,
                                                 size_t switching_pattern_len,
                                                 const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Disable IQ sampling of Silicon Labs CTE.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_disable_silabs_cte();

/** @} */ // end addtogroup sl_bt_cte_receiver

/**
 * @cond RESTRICTED
 * @addtogroup sl_bt_memory_profiler Memory Profiler
 * @{
 *
 * @brief Memory Profiler
 *
 * The commands and events in this class enable reading data collected by the
 * memory profiler and controlling the memory profiler to reset counters and
 * errors.
 */

/* Command and Response IDs */
#define sl_bt_cmd_memory_profiler_get_status_id                      0x00fd0020
#define sl_bt_cmd_memory_profiler_reset_id                           0x01fd0020
#define sl_bt_cmd_memory_profiler_list_ram_usage_id                  0x02fd0020
#define sl_bt_rsp_memory_profiler_get_status_id                      0x00fd0020
#define sl_bt_rsp_memory_profiler_reset_id                           0x01fd0020
#define sl_bt_rsp_memory_profiler_list_ram_usage_id                  0x02fd0020

/**
 * @addtogroup sl_bt_memory_profiler_error_flags Memory profiler error flags
 * @{
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Flags to specify errors detected by the memory profiler. The memory profiler
 * may set other error bits that are not documented here. Error bits that are
 * not documented here indicate internal implementation-specific errors. Their
 * meaning is described in the memory profiler implementation.
 */

/** The number of created trackers has exceeded the configured maximum of
 * SL_MEMORY_PROFILER_MAX_TRACKERS. One or more trackers are therefore missing
 * in the report that can be obtained with the command @ref
 * sl_bt_memory_profiler_list_ram_usage, but their memory usage is included in
 * the total RAM consumption. */
#define SL_BT_MEMORY_PROFILER_ERROR_TOO_MANY_TRACKERS         0x1       

/** The number of live allocations has exceeded the configured maximum of
 * SL_MEMORY_PROFILER_MAX_LIVE_ALLOCATIONS. One or more allocations could not be
 * tracked and their memory usage was not counted. The report that can be
 * obtained with command @ref sl_bt_memory_profiler_list_ram_usage will
 * therefore not truthfully reflect the total memory consumption. */
#define SL_BT_MEMORY_PROFILER_ERROR_TOO_MANY_LIVE_ALLOCATIONS 0x2       

/** @} */ // end Memory profiler error flags

/**
 * @addtogroup sl_bt_memory_profiler_reset_flags Memory profiler reset flags
 * @{
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Flags to specify which memory profiler items to reset
 */

/** Reset the common error flags that can be read with command @ref
 * sl_bt_memory_profiler_get_status. */
#define SL_BT_MEMORY_PROFILER_RESET_ERROR_FLAGS    0x1       

/** Reset the allocation failure counts in every memory tracker. */
#define SL_BT_MEMORY_PROFILER_RESET_ALLOC_FAILURES 0x2       

/** @} */ // end Memory profiler reset flags

/**
 * @addtogroup sl_bt_evt_memory_profiler_ram_usage_entry sl_bt_evt_memory_profiler_ram_usage_entry
 * @{
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Triggered by the command @ref sl_bt_memory_profiler_list_ram_usage
 */

/** @brief Identifier of the ram_usage_entry event */
#define sl_bt_evt_memory_profiler_ram_usage_entry_id                 0x00fd00a0

/***************************************************************************//**
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Data structure of the ram_usage_entry event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_memory_profiler_ram_usage_entry_s
{
  uint32_t   depth;                  /**< Depth in the memory hierarchy */
  uint32_t   total_size;             /**< The total number of bytes allocated
                                          from the parent memory. For memory
                                          pools, this is the total size of the
                                          pool allocated at initialization time.
                                          For memory trackers that are not
                                          memory pools, the total size allocated
                                          from the parent matches the current
                                          number of bytes allocated, i.e., field
                                          bytes_allocated. */
  uint32_t   num_allocations;        /**< Number of allocations currently active */
  uint32_t   num_failed_allocations; /**< Number of allocations that have failed
                                          since boot or when the counters were
                                          last reset with command @ref
                                          sl_bt_memory_profiler_reset. */
  uint32_t   bytes_allocated;        /**< Number of bytes currently allocated */
  uint32_t   peak_bytes_allocated;   /**< Peak number of bytes allocated */
  uint8array description;            /**< Short text describing the RAM usage */
});

typedef struct sl_bt_evt_memory_profiler_ram_usage_entry_s sl_bt_evt_memory_profiler_ram_usage_entry_t;

/** @} */ // end addtogroup sl_bt_evt_memory_profiler_ram_usage_entry

/**
 * @addtogroup sl_bt_evt_memory_profiler_list_ram_usage_complete sl_bt_evt_memory_profiler_list_ram_usage_complete
 * @{
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Triggered by the command @ref sl_bt_memory_profiler_list_ram_usage and
 * follows @ref sl_bt_evt_memory_profiler_ram_usage_entry events
 */

/** @brief Identifier of the list_ram_usage_complete event */
#define sl_bt_evt_memory_profiler_list_ram_usage_complete_id         0x01fd00a0

/** @} */ // end addtogroup sl_bt_evt_memory_profiler_list_ram_usage_complete

/***************************************************************************//**
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Get the status of the memory profiler. The command fails with error code
 * SL_STATUS_NOT_AVAILABLE if the software configuration has not enabled the
 * memory profiling feature.
 *
 * @param[out] num_memory_trackers Number of created memory trackers
 * @param[out] num_live_allocations Number of currently live memory allocations
 * @param[out] peak_live_allocations Peak number of live memory allocations
 * @param[out] num_heap_bytes_used Number of bytes currently used in the heap
 * @param[out] peak_heap_bytes_used Peak number of bytes used in the C heap
 * @param[out] peak_stack_bytes_used Peak number of bytes used in the C stack
 * @param[out] errors Flag bits to indicate detected errors. See @ref
 *   sl_bt_memory_profiler_error_flags for the specified flag bits.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_memory_profiler_get_status(uint32_t *num_memory_trackers,
                                             uint32_t *num_live_allocations,
                                             uint32_t *peak_live_allocations,
                                             uint32_t *num_heap_bytes_used,
                                             uint32_t *peak_heap_bytes_used,
                                             uint32_t *peak_stack_bytes_used,
                                             uint32_t *errors);

/***************************************************************************//**
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Reset the specified items in the memory profiler. The command fails with
 * error code SL_STATUS_NOT_AVAILABLE if the SW configuration has not enabled
 * the memory profiling feature.
 *
 * @param[in] flags Flag bits to specify which items to reset. See @ref
 *   sl_bt_memory_profiler_reset_flags for the available flags.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_memory_profiler_reset(uint32_t flags);

/***************************************************************************//**
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * List the device RAM usage. Entries are reported by the @ref
 * sl_bt_evt_memory_profiler_ram_usage_entry event for each line of RAM usage
 * report and the report is ended with @ref
 * sl_bt_evt_memory_profiler_list_ram_usage_complete event. The command fails
 * with error code SL_STATUS_NOT_AVAILABLE if the SW configuration has not
 * enabled the memory profiling feature. Only one command may be pending at any
 * time. If another command is issued before the previous report is ended with
 * @ref sl_bt_evt_memory_profiler_list_ram_usage_complete event, the command
 * fails with error code SL_STATUS_BUSY.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_memory_profiler_ram_usage_entry
 *   - @ref sl_bt_evt_memory_profiler_list_ram_usage_complete
 *
 ******************************************************************************/
sl_status_t sl_bt_memory_profiler_list_ram_usage();

/** @} */ // end addtogroup sl_bt_memory_profiler
/** @endcond */ // end restricted class

/**
 * @addtogroup sl_bt_user User Messaging
 * @{
 *
 * @brief User Messaging
 *
 * This class provides one command and one event which can be used by a NCP host
 * and target to implement a communication mechanism with a custom proprietary
 * protocol. An application must decide whether and how the command and event
 * are used. The stack does not produce or consume any messages belonging to
 * this class.
 */

/* Command and Response IDs */
#define sl_bt_cmd_user_message_to_target_id                          0x00ff0020
#define sl_bt_cmd_user_manage_event_filter_id                        0x01ff0020
#define sl_bt_rsp_user_message_to_target_id                          0x00ff0020
#define sl_bt_rsp_user_manage_event_filter_id                        0x01ff0020

/**
 * @addtogroup sl_bt_evt_user_message_to_host sl_bt_evt_user_message_to_host
 * @{
 * @brief Used by the target application on a device to initiate communication
 * and send a message to the NCP host
 *
 * Do not send event messages in the context of the user command handling.
 */

/** @brief Identifier of the message_to_host event */
#define sl_bt_evt_user_message_to_host_id                            0x00ff00a0

/***************************************************************************//**
 * @brief Data structure of the message_to_host event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_user_message_to_host_s
{
  uint8array message; /**< The message */
});

typedef struct sl_bt_evt_user_message_to_host_s sl_bt_evt_user_message_to_host_t;

/** @} */ // end addtogroup sl_bt_evt_user_message_to_host

/***************************************************************************//**
 *
 * Used by an NCP host to send a message to the target application on device.
 * The application on the target must send the response with @ref
 * sl_bt_send_rsp_user_message_to_target.
 *
 * @param[in] data_len Length of data in @p data
 * @param[in] data The message
 * @param[in] max_response_size Size of output buffer passed in @p response
 * @param[out] response_len On return, set to the length of output data written
 *   to @p response
 * @param[out] response The response message
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_user_message_to_target(size_t data_len,
                                         const uint8_t* data,
                                         size_t max_response_size,
                                         size_t *response_len,
                                         uint8_t *response);

/***************************************************************************//**
 *
 * Manage NCP event filter. When the event filter is in use, API events passing
 * the filter will be sent to the host, and those that do not pass are discarded
 * by the target. For functionality details, see the NCP component in the
 * Bluetooth SDK.
 *
 * By default, the NCP does not use the event filter.
 *
 * @param[in] data_len Length of data in @p data
 * @param[in] data The message for managing event filter
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_user_manage_event_filter(size_t data_len,
                                           const uint8_t* data);

/** @} */ // end addtogroup sl_bt_user


/***************************************************************************//**
 * @addtogroup sl_bt_common_types BT Common Types
 * @{
 *  @brief BT common types
 */

/**
 * @brief Data structure of BT API messages
 */
PACKSTRUCT( struct sl_bt_msg {
  /** API protocol header consisting of event identifier and data length */
  uint32_t   header;

  /** Union of API event types */
  union {
    uint8_t handle;
    sl_bt_evt_dfu_boot_t                                         evt_dfu_boot; /**< Data field for dfu boot event*/
    sl_bt_evt_dfu_boot_failure_t                                 evt_dfu_boot_failure; /**< Data field for dfu boot_failure event*/
    sl_bt_evt_system_boot_t                                      evt_system_boot; /**< Data field for system boot event*/
    sl_bt_evt_system_error_t                                     evt_system_error; /**< Data field for system error event*/
    sl_bt_evt_system_hardware_error_t                            evt_system_hardware_error; /**< Data field for system hardware_error event*/
    sl_bt_evt_system_external_signal_t                           evt_system_external_signal; /**< Data field for system external_signal event*/
    sl_bt_evt_system_soft_timer_t                                evt_system_soft_timer; /**< Data field for system soft_timer event*/
    sl_bt_evt_advertiser_timeout_t                               evt_advertiser_timeout; /**< Data field for advertiser timeout event*/
    sl_bt_evt_advertiser_scan_request_t                          evt_advertiser_scan_request; /**< Data field for advertiser scan_request event*/
    sl_bt_evt_advertiser_periodic_advertising_status_t           evt_advertiser_periodic_advertising_status; /**< Data field for advertiser periodic_advertising_status event*/
    sl_bt_evt_scanner_scan_report_t                              evt_scanner_scan_report; /**< Data field for scanner scan_report event*/
    sl_bt_evt_sync_opened_t                                      evt_sync_opened; /**< Data field for sync opened event*/
    sl_bt_evt_sync_data_t                                        evt_sync_data; /**< Data field for sync data event*/
    sl_bt_evt_sync_closed_t                                      evt_sync_closed; /**< Data field for sync closed event*/
    sl_bt_evt_connection_opened_t                                evt_connection_opened; /**< Data field for connection opened event*/
    sl_bt_evt_connection_parameters_t                            evt_connection_parameters; /**< Data field for connection parameters event*/
    sl_bt_evt_connection_phy_status_t                            evt_connection_phy_status; /**< Data field for connection phy_status event*/
    sl_bt_evt_connection_rssi_t                                  evt_connection_rssi; /**< Data field for connection rssi event*/
    sl_bt_evt_connection_get_remote_tx_power_completed_t         evt_connection_get_remote_tx_power_completed; /**< Data field for connection get_remote_tx_power_completed event*/
    sl_bt_evt_connection_tx_power_t                              evt_connection_tx_power; /**< Data field for connection tx_power event*/
    sl_bt_evt_connection_remote_tx_power_t                       evt_connection_remote_tx_power; /**< Data field for connection remote_tx_power event*/
    sl_bt_evt_connection_closed_t                                evt_connection_closed; /**< Data field for connection closed event*/
    sl_bt_evt_connection_remote_used_features_t                  evt_connection_remote_used_features; /**< Data field for connection remote_used_features event*/
    sl_bt_evt_gatt_mtu_exchanged_t                               evt_gatt_mtu_exchanged; /**< Data field for gatt mtu_exchanged event*/
    sl_bt_evt_gatt_service_t                                     evt_gatt_service; /**< Data field for gatt service event*/
    sl_bt_evt_gatt_characteristic_t                              evt_gatt_characteristic; /**< Data field for gatt characteristic event*/
    sl_bt_evt_gatt_descriptor_t                                  evt_gatt_descriptor; /**< Data field for gatt descriptor event*/
    sl_bt_evt_gatt_characteristic_value_t                        evt_gatt_characteristic_value; /**< Data field for gatt characteristic_value event*/
    sl_bt_evt_gatt_descriptor_value_t                            evt_gatt_descriptor_value; /**< Data field for gatt descriptor_value event*/
    sl_bt_evt_gatt_procedure_completed_t                         evt_gatt_procedure_completed; /**< Data field for gatt procedure_completed event*/
    sl_bt_evt_gatt_server_attribute_value_t                      evt_gatt_server_attribute_value; /**< Data field for gatt_server attribute_value event*/
    sl_bt_evt_gatt_server_user_read_request_t                    evt_gatt_server_user_read_request; /**< Data field for gatt_server user_read_request event*/
    sl_bt_evt_gatt_server_user_write_request_t                   evt_gatt_server_user_write_request; /**< Data field for gatt_server user_write_request event*/
    sl_bt_evt_gatt_server_characteristic_status_t                evt_gatt_server_characteristic_status; /**< Data field for gatt_server characteristic_status event*/
    sl_bt_evt_gatt_server_execute_write_completed_t              evt_gatt_server_execute_write_completed; /**< Data field for gatt_server execute_write_completed event*/
    sl_bt_evt_gatt_server_indication_timeout_t                   evt_gatt_server_indication_timeout; /**< Data field for gatt_server indication_timeout event*/
    sl_bt_evt_test_dtm_completed_t                               evt_test_dtm_completed; /**< Data field for test dtm_completed event*/
    sl_bt_evt_sm_passkey_display_t                               evt_sm_passkey_display; /**< Data field for sm passkey_display event*/
    sl_bt_evt_sm_passkey_request_t                               evt_sm_passkey_request; /**< Data field for sm passkey_request event*/
    sl_bt_evt_sm_confirm_passkey_t                               evt_sm_confirm_passkey; /**< Data field for sm confirm_passkey event*/
    sl_bt_evt_sm_bonded_t                                        evt_sm_bonded; /**< Data field for sm bonded event*/
    sl_bt_evt_sm_bonding_failed_t                                evt_sm_bonding_failed; /**< Data field for sm bonding_failed event*/
    sl_bt_evt_sm_confirm_bonding_t                               evt_sm_confirm_bonding; /**< Data field for sm confirm_bonding event*/
    sl_bt_evt_sm_list_bonding_entry_t                            evt_sm_list_bonding_entry; /**< Data field for sm list_bonding_entry event*/
    sl_bt_evt_l2cap_coc_connection_request_t                     evt_l2cap_coc_connection_request; /**< Data field for l2cap coc_connection_request event*/
    sl_bt_evt_l2cap_coc_connection_response_t                    evt_l2cap_coc_connection_response; /**< Data field for l2cap coc_connection_response event*/
    sl_bt_evt_l2cap_coc_le_flow_control_credit_t                 evt_l2cap_coc_le_flow_control_credit; /**< Data field for l2cap coc_le_flow_control_credit event*/
    sl_bt_evt_l2cap_coc_channel_disconnected_t                   evt_l2cap_coc_channel_disconnected; /**< Data field for l2cap coc_channel_disconnected event*/
    sl_bt_evt_l2cap_coc_data_t                                   evt_l2cap_coc_data; /**< Data field for l2cap coc_data event*/
    sl_bt_evt_l2cap_command_rejected_t                           evt_l2cap_command_rejected; /**< Data field for l2cap command_rejected event*/
    sl_bt_evt_cte_receiver_dtm_iq_report_t                       evt_cte_receiver_dtm_iq_report; /**< Data field for cte_receiver dtm_iq_report event*/
    sl_bt_evt_cte_receiver_connection_iq_report_t                evt_cte_receiver_connection_iq_report; /**< Data field for cte_receiver connection_iq_report event*/
    sl_bt_evt_cte_receiver_connectionless_iq_report_t            evt_cte_receiver_connectionless_iq_report; /**< Data field for cte_receiver connectionless_iq_report event*/
    sl_bt_evt_cte_receiver_silabs_iq_report_t                    evt_cte_receiver_silabs_iq_report; /**< Data field for cte_receiver silabs_iq_report event*/
    sl_bt_evt_memory_profiler_ram_usage_entry_t                  evt_memory_profiler_ram_usage_entry; /**< Data field for memory_profiler ram_usage_entry event*/
    sl_bt_evt_user_message_to_host_t                             evt_user_message_to_host; /**< Data field for user message_to_host event*/
    uint8_t payload[SL_BGAPI_MAX_PAYLOAD_SIZE];
  } data;
});

/**
 * @brief Type definition for the data structure of BT API messages
 */
typedef struct sl_bt_msg sl_bt_msg_t;

/** @} */ // end addtogroup sl_bt_common_types
/******************************************************************************/

/***************************************************************************//**
 * @addtogroup sl_bt_utility_functions Utility Functions
 *  @brief Utility functions for applications on SoC
 *  @{
 */

/**
 * Get the next event that requires processing by user application. Application
 * is not blocked if no event is waiting.
 *
 * @param event the pointer for storing the new event
 * @return SL_STATUS_OK if a new event is returned, or SL_STATUS_NOT_FOUND if no
 *   event is waiting; other value indicates an error occurred
 */
sl_status_t sl_bt_pop_event(sl_bt_msg_t* event);

/**
 * Check whether events are in queue pending for processing.
 * Call @ref sl_bt_pop_event to process pending events.
 *
 * @return true if event is pending; false otherwise
 */
bool sl_bt_event_pending(void);

/**
 * Check whether events are in queue pending for processing and return the next
 * event length in bytes if events are pending.
 * Call @ref sl_bt_pop_event to process pending events.
 *
 * @return the next event length if event is pending; 0 otherwise
 */
uint32_t sl_bt_event_pending_len(void);


/**
 * Run the Bluetooth stack to process scheduled tasks. Events for user
 * application may be generated as a result of this operation.
 */
void sl_bt_run();

/**
 * Handle an API command in binary format.
 *
 * This is povided to NCP target applications for processing commands received
 * from NCP transport.
 *
 * @param hdr the command header
 * @param data the command payload in a byte array
 */
void sl_bt_handle_command(uint32_t hdr, void* data);

/**
 * Stack internal function used by sl_bt_get_command_response() API.
 */
extern void* sli_bt_get_command_response();

/**
 * Get the response to the command currently been handled.
 *
 * This is provided to NCP target applications for processing commands received
 * from NCP transport.
 */
static inline sl_bt_msg_t* sl_bt_get_command_response()
{
  return (sl_bt_msg_t*) sli_bt_get_command_response();
}

/**
 * Priority message handler function if user application requires the use of
 * PendSV interrupt.
 *
 * If scheduler_callback function pointer in configuration struct is something
 * else than NULL, then stack will not install its own PendSV IRQ handler but
 * instead uses callback/handler functions.
 *
 * When application receives call to the scheduler_callback function it must
 * schedule the call to gecko_priority_handle function to later time to run on
 * high priority thread. This callback may happen inside radio IRQ so processing
 * must not block and has to happen as fast as possible.
 *
 * Recommended implementation: High priority thread is loop that waits on binary
 * semaphore and calls gecko_priority_handler. The scheduler_callback is simple
 * function that only signals the semaphore.
 *
 */
void sl_bt_priority_handle(void);

/**
 * Signals stack that external event has happened. Signals can be used to report
 * status change from interrupt context or from other threads to application.
 * Signals are bits that are automatically cleared after application has been
 * notified.
 *
 * @param signals is a bitmask defining active signals that are reported back to
 *   the application by system_external_signal-event.
 *
 */
void sl_bt_external_signal(uint32_t signals);

/**
 * Signals stack to send system_awake event when application received wakeup
 * signal.
 */
void sl_bt_send_system_awake();

/**
 * Signals stack to send system_error event when in case of an error.
 */
void sl_bt_send_system_error(uint16_t reason, uint8_t data_len, const uint8_t *data);

/**
 * Tells if a SL_BT_API message is sensitive.
 *
 * @param[in] message_header The header of the SL_BT_API message
 * @return 1 if the message is sensitive; otherwise 0
 */
uint8_t sl_bt_is_sensitive_message(uint32_t message_header);

/**
 * Sends the NCP host a message whose SL_BT_MSG_ID is
 * gecko_rsp_user_message_to_target_id.
 *
 * This a utility helping a NCP host and target application to exchange user
 * data. Do not use it in SoC mode.
 */
void sl_bt_send_rsp_user_message_to_target(uint16_t result, uint8_t data_len, uint8_t *data);

/**
 * Sends the NCP host a message whose SL_BT_MSG_ID is
 * gecko_evt_user_message_to_host_id.
 *
 * This a utility helping a NCP host and target application to exchange user
 * data. Do not use it in SoC mode.
 */
void sl_bt_send_evt_user_message_to_host(uint8_t data_len, uint8_t *data);

/**
 * Sends the NCP host a message whose SL_BT_MSG_ID is
 * gecko_rsp_user_manage_event_filter_id.
 *
 * This a utility helping a NCP host and target application to manage event
 * filter. Do not use it in SoC mode.
 */
void sl_bt_send_rsp_user_manage_event_filter(uint16_t result);


/** @} */ // end addtogroup sl_bt_utility_functions
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
