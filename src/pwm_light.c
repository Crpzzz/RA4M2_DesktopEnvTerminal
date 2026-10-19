/*
 * pwm_light.c
 *
 *  Created on: 2026年10月19日
 *      Author: r1072
 */
#include "pwm_light.h"

#include <stdbool.h>

static uint8_t s_pwm_percent = 0U;
static uint32_t s_pwm_period_counts = 0U;

fsp_err_t pwm_light_init(void)
{
    fsp_err_t error;
    timer_info_t timer_info;

    /*
     * 打开 GPT2 PWM。
     */
    error = R_GPT_Open(
        &g_pwm2_ctrl,
        &g_pwm2_cfg
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    /*
     * 获取 GPT2 当前周期对应的计数值。
     */
    error = R_GPT_InfoGet(
        &g_pwm2_ctrl,
        &timer_info
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    s_pwm_period_counts = timer_info.period_counts;

    /*
     * 初始占空比为 0%，灯关闭。
     */
    error = R_GPT_DutyCycleSet(
        &g_pwm2_ctrl,
        0U,
        GPT_IO_PIN_GTIOCA
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    /*
     * 启动 PWM。
     */
    error = R_GPT_Start(
        &g_pwm2_ctrl
    );

    return error;
}

fsp_err_t pwm_light_set_percent(
    uint8_t percent
)
{
    uint32_t duty_counts;

    if (percent > 100U)
    {
        percent = 100U;
    }

    if (s_pwm_period_counts == 0U)
    {
        return FSP_ERR_NOT_OPEN;
    }

    /*
     * 根据百分比换算成 GPT 计数值。
     *
     * 0%   -> 0
     * 50%  -> 周期计数的一半
     * 100% -> 整个周期计数
     */
    duty_counts =
        (s_pwm_period_counts * percent) / 100U;

    if (duty_counts >= s_pwm_period_counts)
    {
        duty_counts = s_pwm_period_counts - 1U;
    }

    fsp_err_t error = R_GPT_DutyCycleSet(
        &g_pwm2_ctrl,
        duty_counts,
        GPT_IO_PIN_GTIOCA
    );

    if (error == FSP_SUCCESS)
    {
        s_pwm_percent = percent;
    }

    return error;
}

uint8_t pwm_light_get_percent(void)
{
    return s_pwm_percent;
}

