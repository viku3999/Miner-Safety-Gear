/***************************************************************************//**
 * @file
 * @brief PWM Driver Instance Initialization
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_pwm.h"

#include "sl_pwm_init_buzzer_msg_config.h"


#include "em_gpio.h"


sl_pwm_instance_t sl_pwm_buzzer_msg = {
  .timer = SL_PWM_BUZZER_MSG_PERIPHERAL,
  .channel = (uint8_t)(SL_PWM_BUZZER_MSG_CHANNEL),
  .port = (uint8_t)(SL_PWM_BUZZER_MSG_PORT),
  .pin = (uint8_t)(SL_PWM_BUZZER_MSG_PIN),
#if defined(SL_PWM_BUZZER_MSG_LOC)
  .location = (uint8_t)(SL_PWM_BUZZER_MSG_LOC),
#endif
};


void sl_pwm_init_instances(void)
{

  sl_pwm_config_t pwm_buzzer_msg_config = {
    .frequency = SL_PWM_BUZZER_MSG_FREQUENCY,
    .polarity = SL_PWM_BUZZER_MSG_POLARITY,
  };

  sl_pwm_init(&sl_pwm_buzzer_msg, &pwm_buzzer_msg_config);

}
