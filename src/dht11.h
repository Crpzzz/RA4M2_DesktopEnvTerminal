/*
 * dht11.h
 *
 *  Created on: 2026年10月18日
 *      Author: r1072
 */

#ifndef DHT11_H
#define DHT11_H

#include "hal_data.h"
#include <stdint.h>

typedef struct
{
    uint8_t humidity;
    uint8_t temperature;
} dht11_data_t;

fsp_err_t dht11_init(void);

fsp_err_t dht11_read(
    dht11_data_t *data
);

#endif/* DHT11_H_ */

