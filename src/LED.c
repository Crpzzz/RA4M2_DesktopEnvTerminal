/*
 * LED.c
 *
 *  Created on: 2026年10月17日
 *      Author: r1072
 */
#include "hal_data.h"
#include "led.h"

static bool s_led_state = false;

void led_init(void)
{
    R_IOPORT_PinWrite(
        &g_ioport_ctrl,
        BSP_IO_PORT_01_PIN_11,
        BSP_IO_LEVEL_LOW
    );

    s_led_state = false;
}

void led_set(bool on)
{
    s_led_state = on;

    R_IOPORT_PinWrite(
        &g_ioport_ctrl,
        BSP_IO_PORT_01_PIN_11,
        on ? BSP_IO_LEVEL_HIGH
            : BSP_IO_LEVEL_LOW
    );
}

void led_toggle(void)
{
    led_set(!s_led_state);
}

