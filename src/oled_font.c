/*
 * oled_font.c
 *
 *  Created on: 2026年10月17日
 *      Author: r1072
 */
#include "oled_font.h"

#include <string.h>

bool oled_font_get(
    char character,
    uint8_t glyph[5]
)
{
    if (glyph == NULL)
    {
        return false;
    }

    /*
     * 先清空 5 列。
     */
    memset(
        glyph,
        0,
        5U
    );

    switch (character)
    {
        case 'A':
            glyph[0] = 0x7EU;
            glyph[1] = 0x11U;
            glyph[2] = 0x11U;
            glyph[3] = 0x11U;
            glyph[4] = 0x7EU;
            break;

        case 'C':
            glyph[0] = 0x3EU;
            glyph[1] = 0x41U;
            glyph[2] = 0x41U;
            glyph[3] = 0x41U;
            glyph[4] = 0x22U;
            break;

        case 'D':
            glyph[0] = 0x7FU;
            glyph[1] = 0x41U;
            glyph[2] = 0x41U;
            glyph[3] = 0x22U;
            glyph[4] = 0x1CU;
            break;

        case 'E':
            glyph[0] = 0x7FU;
            glyph[1] = 0x49U;
            glyph[2] = 0x49U;
            glyph[3] = 0x49U;
            glyph[4] = 0x41U;
            break;

        case 'G':
            glyph[0] = 0x3EU;
            glyph[1] = 0x41U;
            glyph[2] = 0x49U;
            glyph[3] = 0x49U;
            glyph[4] = 0x7AU;
            break;

        case 'H':
            glyph[0] = 0x7FU;
            glyph[1] = 0x08U;
            glyph[2] = 0x08U;
            glyph[3] = 0x08U;
            glyph[4] = 0x7FU;
            break;

        case 'I':
            glyph[0] = 0x00U;
            glyph[1] = 0x41U;
            glyph[2] = 0x7FU;
            glyph[3] = 0x41U;
            glyph[4] = 0x00U;
            break;

        case 'K':
            glyph[0] = 0x7FU;
            glyph[1] = 0x08U;
            glyph[2] = 0x14U;
            glyph[3] = 0x22U;
            glyph[4] = 0x41U;
            break;

        case 'L':
            glyph[0] = 0x7FU;
            glyph[1] = 0x40U;
            glyph[2] = 0x40U;
            glyph[3] = 0x40U;
            glyph[4] = 0x40U;
            break;

        case 'M':
            glyph[0] = 0x7FU;
            glyph[1] = 0x02U;
            glyph[2] = 0x0CU;
            glyph[3] = 0x02U;
            glyph[4] = 0x7FU;
            break;

        case 'N':
            glyph[0] = 0x7FU;
            glyph[1] = 0x04U;
            glyph[2] = 0x08U;
            glyph[3] = 0x10U;
            glyph[4] = 0x7FU;
            break;

        case 'O':
            glyph[0] = 0x3EU;
            glyph[1] = 0x41U;
            glyph[2] = 0x41U;
            glyph[3] = 0x41U;
            glyph[4] = 0x3EU;
            break;

        case 'P':
            glyph[0] = 0x7FU;
            glyph[1] = 0x09U;
            glyph[2] = 0x09U;
            glyph[3] = 0x09U;
            glyph[4] = 0x06U;
            break;

        case 'R':
            glyph[0] = 0x7FU;
            glyph[1] = 0x09U;
            glyph[2] = 0x19U;
            glyph[3] = 0x29U;
            glyph[4] = 0x46U;
            break;

        case 'S':
            glyph[0] = 0x46U;
            glyph[1] = 0x49U;
            glyph[2] = 0x49U;
            glyph[3] = 0x49U;
            glyph[4] = 0x31U;
            break;

        case 'T':
            glyph[0] = 0x01U;
            glyph[1] = 0x01U;
            glyph[2] = 0x7FU;
            glyph[3] = 0x01U;
            glyph[4] = 0x01U;
            break;

        case 'U':
            glyph[0] = 0x3FU;
            glyph[1] = 0x40U;
            glyph[2] = 0x40U;
            glyph[3] = 0x40U;
            glyph[4] = 0x3FU;
            break;

        case 'W':
            glyph[0] = 0x7FU;
            glyph[1] = 0x20U;
            glyph[2] = 0x18U;
            glyph[3] = 0x20U;
            glyph[4] = 0x7FU;
            break;

        case '0':
            glyph[0] = 0x3EU;
            glyph[1] = 0x51U;
            glyph[2] = 0x49U;
            glyph[3] = 0x45U;
            glyph[4] = 0x3EU;
            break;

        case '1':
            glyph[0] = 0x00U;
            glyph[1] = 0x42U;
            glyph[2] = 0x7FU;
            glyph[3] = 0x40U;
            glyph[4] = 0x00U;
            break;

        case '2':
            glyph[0] = 0x62U;
            glyph[1] = 0x51U;
            glyph[2] = 0x49U;
            glyph[3] = 0x49U;
            glyph[4] = 0x46U;
            break;

        case '3':
            glyph[0] = 0x22U;
            glyph[1] = 0x41U;
            glyph[2] = 0x49U;
            glyph[3] = 0x49U;
            glyph[4] = 0x36U;
            break;

        case '4':
            glyph[0] = 0x18U;
            glyph[1] = 0x14U;
            glyph[2] = 0x12U;
            glyph[3] = 0x7FU;
            glyph[4] = 0x10U;
            break;

        case '5':
            glyph[0] = 0x2FU;
            glyph[1] = 0x49U;
            glyph[2] = 0x49U;
            glyph[3] = 0x49U;
            glyph[4] = 0x31U;
            break;

        case '6':
            glyph[0] = 0x3EU;
            glyph[1] = 0x49U;
            glyph[2] = 0x49U;
            glyph[3] = 0x49U;
            glyph[4] = 0x32U;
            break;

        case '7':
            glyph[0] = 0x01U;
            glyph[1] = 0x71U;
            glyph[2] = 0x09U;
            glyph[3] = 0x05U;
            glyph[4] = 0x03U;
            break;

        case '8':
            glyph[0] = 0x36U;
            glyph[1] = 0x49U;
            glyph[2] = 0x49U;
            glyph[3] = 0x49U;
            glyph[4] = 0x36U;
            break;

        case '9':
            glyph[0] = 0x26U;
            glyph[1] = 0x49U;
            glyph[2] = 0x49U;
            glyph[3] = 0x49U;
            glyph[4] = 0x3EU;
            break;

        case ':':
            glyph[0] = 0x00U;
            glyph[1] = 0x36U;
            glyph[2] = 0x36U;
            glyph[3] = 0x00U;
            glyph[4] = 0x00U;
            break;

        case '.':
            glyph[0] = 0x00U;
            glyph[1] = 0x60U;
            glyph[2] = 0x60U;
            glyph[3] = 0x00U;
            glyph[4] = 0x00U;
            break;

        case '%':
            glyph[0] = 0x63U;
            glyph[1] = 0x13U;
            glyph[2] = 0x08U;
            glyph[3] = 0x64U;
            glyph[4] = 0x63U;
            break;

        case '-':
            glyph[0] = 0x08U;
            glyph[1] = 0x08U;
            glyph[2] = 0x08U;
            glyph[3] = 0x08U;
            glyph[4] = 0x08U;
            break;

        case ' ':
            break;

        default:
            return false;
    }

    return true;
}

