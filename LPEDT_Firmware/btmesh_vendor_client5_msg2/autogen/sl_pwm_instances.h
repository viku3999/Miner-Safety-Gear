/***************************************************************************//**
 * @file
 * @brief PWM Driver Instances
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

#ifndef SL_PWM_INSTANCES_H
#define SL_PWM_INSTANCES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_pwm.h"


extern sl_pwm_instance_t sl_pwm_buzzer_msg;

void sl_pwm_init_instances(void);

#ifdef __cplusplus
}
#endif

#endif // SL_PWM_INSTANCES_H
