/*
 * key.c
 *
 *  Created on: 2026年10月19日
 *      Author: r1072
 */
#include "key.h"

#define KEY_IOPORT_CTRL    g_ioport_ctrl

#define KEY_MODE_PIN       BSP_IO_PORT_00_PIN_00
#define KEY_INCREASE_PIN   BSP_IO_PORT_00_PIN_01
#define KEY_DECREASE_PIN   BSP_IO_PORT_04_PIN_02

#define KEY_ACTIVE_LEVEL   BSP_IO_LEVEL_LOW
#define KEY_DEBOUNCE_COUNT 2U

typedef struct
{
    bsp_io_port_pin_t pin;
    uint8_t last_raw;
    uint8_t stable_state;
    uint8_t debounce_count;
    key_event_t event;
} key_item_t;

static key_item_t s_keys[] =
{
    {
        KEY_MODE_PIN,
        1U,
        1U,
        0U,
        KEY_EVENT_MODE
    },
    {
        KEY_INCREASE_PIN,
        1U,
        1U,
        0U,
        KEY_EVENT_INCREASE
    },
    {
        KEY_DECREASE_PIN,
        1U,
        1U,
        0U,
        KEY_EVENT_DECREASE
    }
};

void key_init(void)
{
    /*
     * 这三个引脚已经在 FSP Pins 页面中配置成：
     * GPIO 输入 + 内部上拉。
     *
     * 这里不需要再次配置方向。
     */
}

static bool key_is_pressed(
    bsp_io_port_pin_t pin
)
{
    bsp_io_level_t level = BSP_IO_LEVEL_HIGH;

    R_IOPORT_PinRead(
        &KEY_IOPORT_CTRL,
        pin,
        &level
    );

    return level == KEY_ACTIVE_LEVEL;
}

key_event_t key_scan_10ms(void)
{
    for (uint32_t i = 0U;
         i < (sizeof(s_keys) / sizeof(s_keys[0]));
         i++)
    {
        bool pressed = key_is_pressed(
            s_keys[i].pin
        );

        uint8_t raw_state = pressed ? 0U : 1U;

        /*
         * 原始状态发生变化，重新开始消抖计数。
         */
        if (raw_state != s_keys[i].last_raw)
        {
            s_keys[i].last_raw = raw_state;
            s_keys[i].debounce_count = 0U;
        }
        else
        {
            if (s_keys[i].debounce_count <
                KEY_DEBOUNCE_COUNT)
            {
                s_keys[i].debounce_count++;
            }
        }

        /*
         * 连续两次扫描状态相同，
         * 说明按键状态稳定。
         */
        if ((s_keys[i].debounce_count >=
             KEY_DEBOUNCE_COUNT) &&
            (s_keys[i].stable_state != raw_state))
        {
            s_keys[i].stable_state = raw_state;

            /*
             * 只在“按下瞬间”产生事件，
             * 松开时不产生事件。
             */
            if (raw_state == 0U)
            {
                return s_keys[i].event;
            }
        }
    }

    return KEY_EVENT_NONE;
}

