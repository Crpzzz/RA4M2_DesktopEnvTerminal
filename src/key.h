/*
 * key.h
 *
 *  Created on: 2026年10月19日
 *      Author: r1072
 */

#ifndef KEY_H
#define KEY_H

#include "hal_data.h"
#include <stdint.h>

typedef enum
{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_MODE,
    KEY_EVENT_INCREASE,
    KEY_EVENT_DECREASE
} key_event_t;

void key_init(void);

key_event_t key_scan_10ms(void);

#endif/* KEY_H_ */
