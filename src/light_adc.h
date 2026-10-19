/*
 * light_adc.h
 *
 *  Created on: 2026年10月18日
 *      Author: r1072
 */
#ifndef LIGHT_ADC_H
#define LIGHT_ADC_H

#include "hal_data.h"
#include <stdint.h>

fsp_err_t light_adc_init(void);
fsp_err_t light_adc_read(uint16_t *value);
uint16_t light_adc_read_average(void);

#endif/* LIGHT_ADC_H_ */
