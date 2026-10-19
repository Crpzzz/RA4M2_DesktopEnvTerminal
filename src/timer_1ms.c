/*
 * timer_1ms.c
 *
 *  Created on: 2026年10月17日
 *      Author: r1072
 */
#include "timer_1ms.h"

#include <stdbool.h>

/*
 * 系统时间计数器。
 *
 * GPT0 每产生一次 1 ms 周期中断，
 * 这个变量就增加 1。
 */
static volatile uint32_t s_tick_ms = 0U;

/*
 * GPT0 的中断回调函数。
 *
 * 函数名必须与 FSP 中 Callback 的名字完全一致。
 */
void timer_1ms_callback(timer_callback_args_t *p_args)
{
    /*
     * 只有周期结束事件才更新时间。
     */
    if ((p_args != NULL) &&
        (p_args->event == TIMER_EVENT_CYCLE_END))
    {
        s_tick_ms++;
    }
}

/*
 * 初始化并启动 GPT0。
 */
fsp_err_t timer_1ms_init(void)
{
    fsp_err_t error;

    /*
     * 打开 GPT0。
     *
     * g_timer0_ctrl 和 g_timer0_cfg
     * 是 FSP 根据 Name 自动生成的对象。
     */
    error = R_GPT_Open(
        &g_timer0_ctrl,
        &g_timer0_cfg
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    /*
     * 启动 GPT0。
     *
     * 启动后，每 1 ms 进入一次
     * timer_1ms_callback()。
     */
    error = R_GPT_Start(
        &g_timer0_ctrl
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    return FSP_SUCCESS;
}

/*
 * 获取当前系统运行时间。
 */
uint32_t timer_1ms_get(void)
{
    return s_tick_ms;
}

