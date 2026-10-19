/*
 * oled_font.h
 *
 *  Created on: 2026年10月17日
 *      Author: r1072
 */

#ifndef OLED_FONT_H_
#define OLED_FONT_H_

#include <stdbool.h>
#include <stdint.h>

bool oled_font_get(
    char character,
    uint8_t glyph[5]
);

#endif /* OLED_FONT_H_ */
