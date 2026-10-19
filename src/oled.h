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
#include <stdint.h>

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

#endif
/* OLED_H_ */
