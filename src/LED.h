/*
 * LED.h
 *
 *  Created on: 2026年10月17日
 *      Author: r1072
 */

#ifndef LED_H_
#define LED_H_

#include <stdbool.h>

void led_init(void);
void led_set(bool on);
void led_toggle(void);

#endif /* LED_H_ */
