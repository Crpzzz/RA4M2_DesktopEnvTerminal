/*
 * oled.h
 *
 *  Created on: 2026年10月17日
 *      Author: r1072
 */

#ifndef OLED_H
#define OLED_H

#include "hal_data.h"
#include <stdint.h>
#include <stdbool.h>

fsp_err_t oled_init(void);

fsp_err_t oled_clear(void);

fsp_err_t oled_show_startup(void);

fsp_err_t oled_show_light_adc(uint16_t adc_value);

fsp_err_t oled_show_environment(
    bool dht_valid,
    uint8_t temperature,
    uint8_t humidity,
    uint16_t light_adc
);

fsp_err_t oled_show_control(
    uint16_t light_adc,
    uint8_t pwm_percent,
    bool auto_mode
);

fsp_err_t oled_show_control_percent(
    uint8_t light_percent,
    uint8_t target_percent,
    uint8_t pwm_percent,
    bool auto_mode
);
/* 显示光照、目标值、温湿度、PWM 和当前模式。 */
fsp_err_t oled_show_status_percent(
    uint8_t light_percent,
    uint8_t target_percent,
    uint8_t pwm_percent,
    bool auto_mode,
    bool dht_valid,
    uint8_t temperature,
    uint8_t humidity
);

#endif
/* OLED_H_ */

