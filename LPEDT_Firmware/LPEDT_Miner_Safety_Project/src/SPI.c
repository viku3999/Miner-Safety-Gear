/*
 * SPI.c
 *
 *  Created on: Oct 12, 2024
 *      Author: vishn
 */

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

#include "GPIO.h"
#include "SPI.h"

#include "spidrv.h"

// Include logging specifically for this .c file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

uint8_t TxBuffer[TX_BUFFER_SIZE] = {0x80};
uint8_t RxBuffer[RX_BUFFER_SIZE];

SPIDRV_HandleData_t handleData;
SPIDRV_Handle_t handle = &handleData;

uint8_t buffer[1] = {0x80};
uint8_t buffer_rx[2];
SPIDRV_Init_t initData = SPIDRV_MASTER_USART1;

/**************************************************************************//**
 * @brief Initialize USART1
 *****************************************************************************/
void initUSART0 (void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_USART1, true);

  // Configure GPIO mode
//  GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 0); // US1_CLK is push pull
//  GPIO_PinModeSet(gpioPortC, 9, gpioModePushPull, 1); // US1_CS is push pull
//  GPIO_PinModeSet(gpioPortC, 6, gpioModePushPull, 1); // US1_TX (MOSI) is push pull
//  GPIO_PinModeSet(gpioPortC, 7, gpioModeInput, 1);    // US1_RX (MISO) is input

  // Start with default config, then modify as necessary
  USART_InitSync_TypeDef config = USART_INITSYNC_DEFAULT;
  config.master       = true;            // master mode
  config.baudrate     = 1000000;         // CLK freq is 1 MHz
  config.autoCsEnable = false;            // CS pin controlled by hardware, not firmware
  config.clockMode    = usartClockMode0; // clock idle low, sample on rising/first edge
  config.msbf         = true;            // send MSB first
  config.enable       = usartDisable;    // Make sure to keep USART disabled until it's all set up
  USART_InitSync(USART1, &config);

  // Set USART pin locations
  USART1->ROUTELOC0 = (USART_ROUTELOC0_CLKLOC_LOC11) | // US1_CLK       on location 11 = PC8 per datasheet section 6.4 = EXP Header pin 8
                      (USART_ROUTELOC0_CSLOC_LOC11)  | // US1_CS        on location 11 = PC9 per datasheet section 6.4 = EXP Header pin 10
                      (USART_ROUTELOC0_TXLOC_LOC11)  | // US1_TX (MOSI) on location 11 = PC6 per datasheet section 6.4 = EXP Header pin 4
                      (USART_ROUTELOC0_RXLOC_LOC11);   // US1_RX (MISO) on location 11 = PC7 per datasheet section 6.4 = EXP Header pin 6

  // Enable USART pins
  USART1->ROUTEPEN = USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_CSPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN;

  // Enable USART1
  USART_Enable(USART1, usartEnable);
}

void Send_tx(void)
{
  uint32_t i;
    for (i = 0; i < TX_BUFFER_SIZE; i++)
    {
     RxBuffer[i] = USART_SpiTransfer(USART1, TxBuffer[i]);
     RxBuffer[i] = USART_Rx(USART1);
     LOG_INFO("returned: %d\r\n", (uint8_t) RxBuffer[i]);
    }
    // Place breakpoint here and observe RxBuffer
    // RxBuffer should contain 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9
}


void TransferComplete(SPIDRV_Handle_t handle,
                      Ecode_t transferStatus,
                      int itemsTransferred)
{
  if (transferStatus == ECODE_EMDRV_SPIDRV_OK) {
   // Success !
//      SPIDRV_MReceiveB(handle, &buffer_rx[0], 1);
//      SPIDRV_MReceiveB(handle, &buffer_rx[1], 1);
//      LOG_INFO("returned: %d, %d\r\n", (uint8_t) RxBuffer[0], (uint8_t) buffer_rx[1]);
      LOG_INFO("Tx success\r\n");
  }
}

void SPI_Trial_Init(){

  // Initialize an SPI driver instance.
  SPIDRV_Init(handle, &initData);
}

void SPI_Trial(){
  // Transmit data using a blocking transmit function.
//  SPIDRV_MTransmitB(handle, buffer, 10);

  // Transmit data using a callback to catch transfer completion.
//  SPIDRV_MTransmit(handle, buffer, 1, TransferComplete);
  uint8_t txData[2];  // TX buffer
  uint8_t rxData[2];  // RX buffer

  txData[0] = 0x00 | 0x80;  // Set the read bit
  txData[1] = 0;           // Dummy byte for the read

  // Transmit and receive
  SPIDRV_MTransfer(handle, txData, rxData, sizeof(txData), TransferComplete);

  LOG_INFO("returned: %d, %d\r\n", (uint8_t) rxData[0], (uint8_t) rxData[1]);
}


void SPI_Init(){
//    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(cmuClock_USART1, true);

    // Configure GPIO mode
//    GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 0); // US1_CLK is push pull
//    GPIO_PinModeSet(gpioPortC, 9, gpioModePushPull, 1); // US1_CS is push pull
//    GPIO_PinModeSet(gpioPortC, 6, gpioModePushPull, 1); // US1_TX (MOSI) is push pull
//    GPIO_PinModeSet(gpioPortC, 7, gpioModeInput, 1);    // US1_RX (MISO) is input

    // Start with default config, then modify as necessary
    USART_InitSync_TypeDef config = USART_INITSYNC_DEFAULT;
    config.master       = true;            // master mode
    config.baudrate     = 1000000;         // CLK freq is 1 MHz
    config.autoCsEnable = false;            // CS pin controlled by hardware, not firmware
    config.clockMode    = usartClockMode0; // clock idle low, sample on rising/first edge
    config.msbf         = true;            // send MSB first
    config.enable       = usartDisable;    // Make sure to keep USART disabled until it's all set up
    USART_InitSync(USART1, &config);

    // Set USART pin locations
    USART1->ROUTELOC0 = (USART_ROUTELOC0_CLKLOC_LOC11) | // US1_CLK       on location 11 = PC8 per datasheet section 6.4 = EXP Header pin 8
                    //    (USART_ROUTELOC0_CSLOC_LOC11)  | // US1_CS        on location 11 = PC9 per datasheet section 6.4 = EXP Header pin 10
                        (USART_ROUTELOC0_TXLOC_LOC11)  | // US1_TX (MOSI) on location 11 = PC6 per datasheet section 6.4 = EXP Header pin 4
                        (USART_ROUTELOC0_RXLOC_LOC11);   // US1_RX (MISO) on location 11 = PC7 per datasheet section 6.4 = EXP Header pin 6

    // Enable USART pins
//    USART1->ROUTEPEN = USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_CSPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN;

    // Enable USART pins
    USART1->ROUTEPEN = USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN;

    // Enable USART1
    USART_Enable(USART1, usartEnable);
}

void SPI_Get_Chip_Id(){

  uint8_t TxBuffer = {0x00};
  uint8_t RxBuffer;
  gpioSpiCs(0);
  RxBuffer = USART_SpiTransfer(USART1, TxBuffer);
  gpioSpiCs(1);
  LOG_INFO("Got: %02x", RxBuffer);
}
