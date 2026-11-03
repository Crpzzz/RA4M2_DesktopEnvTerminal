/*
 * light_level.c
 *
 *  Created on: 2026年10月20日
 *      Author: r1072
 */

#include "light_level.h"

static uint16_t s_adc_dark = 0U;
static uint16_t s_adc_bright = 4095U;
static uint8_t s_target_percent = 60U;

static uint16_t limit_adc(
    uint16_t value,
    uint16_t minimum,
    uint16_t maximum
)
{
    if (value < minimum)
    {
        return minimum;
    }

    if (value > maximum)
    {
        return maximum;
    }

    return value;
}

void light_level_init(
    uint16_t adc_dark,
    uint16_t adc_bright
)
{
    s_adc_dark = adc_dark;
    s_adc_bright = adc_bright;
}

uint8_t light_level_convert(uint16_t adc_value)
{
    uint32_t percent;

    /*
     * 你的传感器是：
     * ADC 越小，实际光线越强。
     *
     * 因此这里使用：
     * adc_dark - adc_value
     */
    adc_value = limit_adc(
        adc_value,
        s_adc_bright,
        s_adc_dark
    );

    if (s_adc_dark <= s_adc_bright)
    {
        return 0U;
    }

    percent =
        ((uint32_t)(s_adc_dark - adc_value) * 100U) /
        (uint32_t)(s_adc_dark - s_adc_bright);

    if (percent > 100U)
    {
        percent = 100U;
    }

    return (uint8_t) percent;
}

uint8_t light_level_get_target(void)
{
    return s_target_percent;
}

void light_level_set_target(uint8_t target)
{
    if (target > 100U)
    {
        target = 100U;
    }

    s_target_percent = target;
}

