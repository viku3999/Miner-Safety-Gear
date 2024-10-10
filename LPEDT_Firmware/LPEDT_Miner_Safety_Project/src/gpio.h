/*
   gpio.h
  
    Created on: Dec 12, 2018
        Author: Dan Walkes

    Updated by Dave Sluiter Sept 7, 2020. moved #defines from .c to .h file.
    Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.

    Editor: Feb 26, 2022, Dave Sluiter
    Change: Added comment about use of .h files.

 *
 * Student edit: Add your name and email address here:
 * @student    Vishnu Kumar, Vishnu.Kumar-1@Colorado.edu
 *
 
 */


// Students: Remember, a header file (a .h file) generally defines an interface
//           for functions defined within an implementation file (a .c file).
//           The .h file defines what a caller (a user) of a .c file requires.
//           At a minimum, the .h file should define the publicly callable
//           functions, i.e. define the function prototypes. #define and type
//           definitions can be added if the caller requires theses.


#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_




// Function prototypes
void gpioInit();

/**
 * @brief   returns the current state of the PB0 button
 * @return  PB0 button state as a boolean
 */
bool Get_PB0_State();

/**
 * @brief   returns the current state of the PB1 button
 * @return  PB1 button state as a boolean
 */
bool Get_PB1_State();

/**
 * @brief   Sets the GPIO pin connected to LED0 HIGH
 * @return  none
 */
void gpioLed0SetOn();

/**
 * @brief   Sets the GPIO pin connected to LED0 LOW
 * @return  none
 */
void gpioLed0SetOff();

/**
 * @brief   Sets the GPIO pin connected to LED1 HIGH
 * @return  none
 */
void gpioLed1SetOn();

/**
 * @brief   Sets the GPIO pin connected to LED1 LOW
 * @return  none
 */
void gpioLed1SetOff();

/**
 * @brief   Init the GPIO drive strengths and modes of operation of the pin
 *          connected to the enable port for the Si7021 sensor
 * @return  none
 */
void Si7021GPIOInit();

/**
 * @brief   Sets the GPIO pin connected to Enable pin of Si7021 sensor HIGH
 * @return  none
 */
void si7021TurnOn();

/**
 * @brief   Sets the GPIO pin connected to Enable pin of Si7021 sensor LOW
 * @return  none
 */
void si7021TurnOff();

/**
 * @brief   Sets the EXTCOMIN pin of the LCD with the given value
 * @param   state The EXTCOMIN pin state.
 *                Acceptable values are either true or false
 * @return  none
 */
void gpioSetDisplayExtcomin(bool state);

#endif /* SRC_GPIO_H_ */
