/*
 * pwm_light.h
 *
 *  Created on: 2026年10月19日
 *      Author: r1072
 */

#ifndef PWM_LIGHT_H_
#define PWM_LIGHT_H_

#include "hal_data.h"
#include <stdint.h>

fsp_err_t pwm_light_init(void);

fsp_err_t pwm_light_set_percent(
    uint8_t percent
);

uint8_t pwm_light_get_percent(void);


#endif /* PWM_LIGHT_H_ */
