/*
 * light_level.h
 *
 *  Created on: 2026年10月20日
 *      Author: r1072
 */

#ifndef LIGHT_LEVEL_H
#define LIGHT_LEVEL_H

#include <stdint.h>

void light_level_init(
    uint16_t adc_dark,
    uint16_t adc_bright
);

uint8_t light_level_convert(
    uint16_t adc_value
);

uint8_t light_level_get_target(void);

void light_level_set_target(
    uint8_t target
);


#endif /* LIGHT_LEVEL_H_ */
