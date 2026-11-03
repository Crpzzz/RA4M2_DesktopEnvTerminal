/*
 * key.c
 *
 *  Created on: 2026年10月19日
 *      Author: r1072
 */
#include "key.h"

/*
 * 按键按下时接地，因此：
 * 高电平 = 松开
 * 低电平 = 按下
 */
#define KEY_PRESSED_LEVEL BSP_IO_LEVEL_LOW

/* 三个按键引脚 */
#define KEY_MODE_PIN     BSP_IO_PORT_00_PIN_00   /* P000 */
#define KEY_INCREASE_PIN BSP_IO_PORT_00_PIN_01   /* P001 */
#define KEY_DECREASE_PIN BSP_IO_PORT_04_PIN_02   /* P402 */

typedef struct
{
    bsp_io_port_pin_t pin;
    bool stable_pressed;
    bool event_reported;
    uint8_t same_count;
} key_state_t;

static key_state_t s_mode;
static key_state_t s_increase;
static key_state_t s_decrease;

/* 读取某一个按键是否被按下 */
static bool key_read_pressed(bsp_io_port_pin_t pin)
{
    bsp_io_level_t level = BSP_IO_LEVEL_HIGH;

    if (R_IOPORT_PinRead(&g_ioport_ctrl, pin, &level) != FSP_SUCCESS)
    {
        /* 读取失败时，按未按下处理 */
        return false;
    }

    return (level == KEY_PRESSED_LEVEL);
}

/*
 * 按键消抖。
 * 每 10 ms 调用一次，连续两次检测到状态变化，
 * 才确认按键真正发生了变化。
 */
static bool key_update(key_state_t *key)
{
    bool pressed = key_read_pressed(key->pin);

    if (pressed == key->stable_pressed)
    {
        /* 当前状态没有变化 */
        key->same_count = 0U;
    }
    else
    {
        /*
         * 状态变化后连续检测两次。
         * 两次约等于 20 ms，可以过滤机械抖动。
         */
        if (key->same_count < 2U)
        {
            key->same_count++;
        }

        if (key->same_count >= 2U)
        {
            key->stable_pressed = pressed;
            key->same_count = 0U;

            /*
             * 松开按键后，允许下一次按下再次触发事件。
             */
            if (!pressed)
            {
                key->event_reported = false;
            }
        }
    }

    /*
     * 只在确认按下的瞬间返回一次事件。
     * 长按不会连续触发。
     */
    if (key->stable_pressed && !key->event_reported)
    {
        key->event_reported = true;
        return true;
    }

    return false;
}

void key_init(void)
{
    s_mode.pin = KEY_MODE_PIN;
    s_mode.stable_pressed = false;
    s_mode.event_reported = false;
    s_mode.same_count = 0U;

    s_increase.pin = KEY_INCREASE_PIN;
    s_increase.stable_pressed = false;
    s_increase.event_reported = false;
    s_increase.same_count = 0U;

    s_decrease.pin = KEY_DECREASE_PIN;
    s_decrease.stable_pressed = false;
    s_decrease.event_reported = false;
    s_decrease.same_count = 0U;
}

key_event_t key_scan_10ms(void)
{
    /*
     * 一次扫描最多返回一个事件。
     * 模式键优先级最高。
     */
    if (key_update(&s_mode))
    {
        return KEY_EVENT_MODE;
    }

    if (key_update(&s_increase))
    {
        return KEY_EVENT_INCREASE;
    }

    if (key_update(&s_decrease))
    {
        return KEY_EVENT_DECREASE;
    }

    return KEY_EVENT_NONE;
}

