/*
  gpio.c
 
   Created on: Dec 12, 2018
       Author: Dan Walkes
   Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.

   March 17
   Dave Sluiter: Use this file to define functions that set up or control GPIOs.
   
   Jan 24, 2023
   Dave Sluiter: Cleaned up gpioInit() to make it less confusing for students regarding
                 drive strength setting. 

 *
 * Student edit: Add your name and email address here:
 * @student    Vishnu Kumar, Vishnu.Kumar-1@Colorado.edu
 *
 
 */


// *****************************************************************************
// Students:
// We will be creating additional functions that configure and manipulate GPIOs.
// For any new GPIO function you create, place that function in this file.
// *****************************************************************************

#include <stdbool.h>
#include "em_gpio.h"
#include <string.h>

#include "gpio.h"


// Student Edit: Define these, 0's are placeholder values.
//
// See the radio board user guide at https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf
// and GPIO documentation at https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/group__GPIO.html
// to determine the correct values for these.
// If these links have gone bad, consult the reference manual and/or the datasheet for the MCU.
// Change to correct port and pins:
#define LED_port   (gpioPortF)
#define LED0_pin   (4)
#define LED1_pin   (5)

#define SI7021_I2C_PORT (gpioPortC)
#define SI7021_EN_PORT (gpioPortD)
#define SI7021_EN_PIN (15)

#define LCD_EXTCOMIN_PORT (gpioPortD)
#define LCD_EXTCOMIN_PIN (13)

#define PB0_PORT  (gpioPortF)
#define PB1_PORT  (gpioPortF)
#define PB0_PIN (6)
#define PB1_PIN (7)

#define SPI_CS_PORT (gpioPortC)
#define SPI_CS_PIN  (9)

bool PB0_State;
bool PB1_State;

// Set GPIO drive strengths and modes of operation
void gpioInit()
{
    // Set the port's drive strength. In this MCU implementation, all GPIO cells
    // in a "Port" share the same drive strength setting. 
//	GPIO_DriveStrengthSet(LED_port, gpioDriveStrengthStrongAlternateStrong); // Strong, 10mA
	GPIO_DriveStrengthSet(LED_port, gpioDriveStrengthWeakAlternateWeak); // Weak, 1mA
  GPIO_DriveStrengthSet(SI7021_EN_PORT, gpioDriveStrengthWeakAlternateWeak); // Weak, 1mA
	GPIO_DriveStrengthSet(LCD_EXTCOMIN_PORT, gpioDriveStrengthWeakAlternateWeak); // Weak, 1mA
  GPIO_DriveStrengthSet(PB0_PORT, gpioDriveStrengthWeakAlternateWeak); // Weak, 1mA
  GPIO_DriveStrengthSet(PB1_PORT, gpioDriveStrengthWeakAlternateWeak); // Weak, 1mA
	
	// Set the 2 GPIOs mode of operation
	GPIO_PinModeSet(LED_port, LED0_pin, gpioModePushPull, false);
	GPIO_PinModeSet(LED_port, LED1_pin, gpioModePushPull, false);

  GPIO_PinModeSet(LCD_EXTCOMIN_PORT, LCD_EXTCOMIN_PIN, gpioModePushPull, false);

  // Setting the 2 pushbuttons to input mode, pulled up with filtering
  GPIO_PinModeSet(PB0_PORT, PB0_PIN, gpioModeInputPullFilter, true);
  GPIO_PinModeSet(PB1_PORT, PB1_PIN, gpioModeInputPullFilter, true);

  // Configure GPIO for SPI
  GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 0); // US1_CLK is push pull
  GPIO_PinModeSet(gpioPortC, 9, gpioModePushPull, 1); // US1_CS is push pull
  GPIO_PinModeSet(gpioPortC, 6, gpioModePushPull, 1); // US1_TX (MOSI) is push pull
  GPIO_PinModeSet(gpioPortC, 7, gpioModeInput, 1);    // US1_RX (MISO) is input

  // Enable IRQ for even numbered GPIO pins
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  // Enable IRQ for odd numbered GPIO pins
  NVIC_EnableIRQ(GPIO_ODD_IRQn);

  // setup interrupt for the input ports
  GPIO_ExtIntConfig (PB0_PORT, PB0_PIN, PB0_PIN, true, true, true);
  GPIO_ExtIntConfig (PB1_PORT, PB1_PIN, PB1_PIN, true, true, true);

  PB0_State = false;
  PB1_State = false;

} // gpioInit()

/**
 * @brief   returns the current state of the PB1 button
 * @return  PB1 button state as a boolean
 */
bool Get_PB1_State(){
//  return PB1_State;
  return !GPIO_PinInGet(PB1_PORT, PB1_PIN);
}

/**
 * @brief   returns the current state of the PB0 button
 * @return  PB0 button state as a boolean
 */
bool Get_PB0_State(){
//  return PB0_State;
  return !GPIO_PinInGet(PB0_PORT, PB0_PIN);
}

/**
 * @brief   Init the GPIO drive strengths and modes of operation of the pin
 *          connected to the enable port for the Si7021 sensor
 * @return  none
 */
void Si7021GPIOInit(){
  GPIO_DriveStrengthSet(SI7021_EN_PORT, gpioDriveStrengthWeakAlternateWeak); // Weak, 1mA
  GPIO_PinModeSet(SI7021_EN_PORT, SI7021_EN_PIN, gpioModePushPull, false);
}

void gpioSpiCs(int x){
  if(x == 0){
    GPIO_PinOutClear(SPI_CS_PORT, SPI_CS_PIN);
  }

  if(x == 1){
    GPIO_PinOutSet(SPI_CS_PORT, SPI_CS_PIN);
  }

}

/**
 * @brief   Sets the GPIO pin connected to LED0 HIGH
 * @return  none
 */
void gpioLed0SetOn()
{
	GPIO_PinOutSet(LED_port, LED0_pin);
}


/**
 * @brief   Sets the GPIO pin connected to LED0 LOW
 * @return  none
 */
void gpioLed0SetOff()
{
	GPIO_PinOutClear(LED_port, LED0_pin);
}


/**
 * @brief   Sets the GPIO pin connected to LED1 HIGH
 * @return  none
 */
void gpioLed1SetOn()
{
	GPIO_PinOutSet(LED_port, LED1_pin);
}

/**
 * @brief   Sets the GPIO pin connected to LED1 LOW
 * @return  none
 */
void gpioLed1SetOff()
{
	GPIO_PinOutClear(LED_port, LED1_pin);
}

/**
 * @brief   Sets the GPIO pin connected to Enable pin of Si7021 sensor HIGH
 * @return  none
 */
void si7021TurnOn(){
  GPIO_PinOutSet(SI7021_EN_PORT, SI7021_EN_PIN);
}

/**
 * @brief   Sets the GPIO pin connected to Enable pin of Si7021 sensor LOW
 * @return  none
 */
void si7021TurnOff(){
  // Commenting out this code post A5 to ensure the on-board LCD stays on
//  GPIO_PinOutClear(SI7021_EN_PORT, SI7021_EN_PIN);
}

/**
 * @brief   Sets the EXTCOMIN pin of the LCD with the given value
 * @param   state The EXTCOMIN pin state.
 *                Acceptable values are either true or false
 * @return  none
 */
void gpioSetDisplayExtcomin(bool state){
  if(state == true)
    GPIO_PinOutSet(LCD_EXTCOMIN_PORT, LCD_EXTCOMIN_PIN);
  else
    GPIO_PinOutClear(LCD_EXTCOMIN_PORT, LCD_EXTCOMIN_PIN);
}





