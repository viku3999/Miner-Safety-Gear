#ifndef PWM_INIT_BUZZER_MSG_CONFIG_H
#define PWM_INIT_BUZZER_MSG_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

// <<< Use Configuration Wizard in Context Menu >>>

// <h>PWM configuration

// <o SL_PWM_BUZZER_MSG_FREQUENCY> PWM frequency [Hz]
// <i> Default: 10000
#define SL_PWM_BUZZER_MSG_FREQUENCY       200

// <o SL_PWM_BUZZER_MSG_POLARITY> Polarity
// <PWM_ACTIVE_HIGH=> Active high
// <PWM_ACTIVE_LOW=> Active low
// <i> Default: PWM_ACTIVE_HIGH
#define SL_PWM_BUZZER_MSG_POLARITY        PWM_ACTIVE_HIGH
// </h> end pwm configuration

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>

// <timer channel=OUTPUT> SL_PWM_BUZZER_MSG
// $[TIMER_SL_PWM_BUZZER_MSG]
#define SL_PWM_BUZZER_MSG_PERIPHERAL             TIMER0
#define SL_PWM_BUZZER_MSG_PERIPHERAL_NO          0

#define SL_PWM_BUZZER_MSG_OUTPUT_CHANNEL         0
// TIMER0 CC0 on PA5
#define SL_PWM_BUZZER_MSG_OUTPUT_PORT            gpioPortA
#define SL_PWM_BUZZER_MSG_OUTPUT_PIN             5
#define SL_PWM_BUZZER_MSG_OUTPUT_LOC             5
// [TIMER_SL_PWM_BUZZER_MSG]$

// <<< sl:end pin_tool >>>

#ifdef __cplusplus
}
#endif

#endif // PWM_INIT_BUZZER_MSG_CONFIG_H
