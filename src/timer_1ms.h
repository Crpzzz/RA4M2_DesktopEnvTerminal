/*
 * timer_1ms.h
 *
 *  Created on: 2026年10月17日
 *      Author: r1072
 */
#ifndef TIMER_1MS_H
#define TIMER_1MS_H

#include "hal_data.h"
#include <stdint.h>

/*
 * 初始化并启动 1 ms GPT 定时器。
 */
fsp_err_t timer_1ms_init(void);

/*
 * 获取系统运行时间，单位为毫秒。
 */
uint32_t timer_1ms_get(void);

#endif
