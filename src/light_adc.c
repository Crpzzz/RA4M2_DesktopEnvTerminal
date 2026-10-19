/*
 * light_adc.c
 *
 *  Created on: 2026年10月18日
 *      Author: r1072
 */
#include "light_adc.h"

#include <stdbool.h>
#include <stddef.h>

#define LIGHT_ADC_CHANNEL       ADC_CHANNEL_12
#define LIGHT_ADC_SAMPLE_COUNT  (8U)
#define LIGHT_ADC_MAX_VALUE     (4095U)

static volatile bool s_scan_complete = false;

static uint16_t s_samples[LIGHT_ADC_SAMPLE_COUNT];
static uint32_t s_sample_sum = 0U;
static uint8_t s_sample_index = 0U;
static uint8_t s_sample_count = 0U;

/*
 * FSP 完成一次 ADC 扫描时调用此回调。
 * 回调只设置标志，不在中断中打印或处理数据。
 */
void light_adc_callback(adc_callback_args_t *p_args)
{
    if ((p_args != NULL) &&
        (p_args->event == ADC_EVENT_SCAN_COMPLETE))
    {
        s_scan_complete = true;
    }
}

/*
 * 打开 ADC，并配置 FSP 生成的 AN012 通道。
 */
fsp_err_t light_adc_init(void)
{
    fsp_err_t error;

    error = R_ADC_Open(
        &g_adc0_ctrl,
        &g_adc0_cfg
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    error = R_ADC_ScanCfg(
        &g_adc0_ctrl,
        &g_adc0_channel_cfg
    );

    return error;
}

/*
 * 触发一次 AN012 采样，并等待转换完成。
 */
fsp_err_t light_adc_read(uint16_t *value)
{
    fsp_err_t error;
    uint32_t timeout_count = 0U;

    if (value == NULL)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    s_scan_complete = false;

    error = R_ADC_ScanStart(&g_adc0_ctrl);
    if (error != FSP_SUCCESS)
    {
        return error;
    }

    /*
     * 最多等待约 100 ms，避免扫描中断未到时无限卡住。
     */
    while ((!s_scan_complete) && (timeout_count < 1000U))
    {
        R_BSP_SoftwareDelay(
            100U,
            BSP_DELAY_UNITS_MICROSECONDS
        );

        timeout_count++;
    }

    if (!s_scan_complete)
    {
        (void) R_ADC_ScanStop(&g_adc0_ctrl);
        return FSP_ERR_TIMEOUT;
    }

    return R_ADC_Read(
        &g_adc0_ctrl,
        LIGHT_ADC_CHANNEL,
        value
    );
}

/*
 * 读取一次并维护最近 8 次采样的滑动平均值。
 */
uint16_t light_adc_read_average(void)
{
    uint16_t raw = 0U;

    if (light_adc_read(&raw) != FSP_SUCCESS)
    {
        return 0U;
    }

    if (s_sample_count < LIGHT_ADC_SAMPLE_COUNT)
    {
        s_samples[s_sample_index] = raw;
        s_sample_sum += raw;
        s_sample_count++;
    }
    else
    {
        s_sample_sum -= s_samples[s_sample_index];
        s_samples[s_sample_index] = raw;
        s_sample_sum += raw;
    }

    s_sample_index++;
    if (s_sample_index >= LIGHT_ADC_SAMPLE_COUNT)
    {
        s_sample_index = 0U;
    }

    return (uint16_t)(s_sample_sum / s_sample_count);
}

