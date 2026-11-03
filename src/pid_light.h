/*
 * pid_light.h
 *
 *  Created on: 2026年10月20日
 *      Author: r1072
 */

#ifndef PID_LIGHT_H
#define PID_LIGHT_H

/*
 * PI 控制器的数据。
 *
 * 输入：目标光照、实际光照，单位都是相对百分比。
 * 输出：PWM 占空比，范围为 0~100。
 */
typedef struct
{
    float kp;          /* 比例增益 */
    float ki;          /* 积分增益 */

    float integral;    /* 积分项对输出的贡献 */
    float output;      /* 最近一次计算出的输出 */
} pid_light_t;

/*
 * 手动切换到自动时调用。
 *
 * output 传入当前 PWM，让自动控制从现有亮度开始。
 */
void pid_light_start(
    pid_light_t *pid,
    float target,
    float measured,
    float output
);

/*
 * 自动模式下，约每 100 ms 调用一次。
 *
 * dt：本次与上次控制之间的时间，单位为秒。
 */
float pid_light_update(
    pid_light_t *pid,
    float target,
    float measured,
    float dt
);

#endif /* PID_LIGHT_H_ */
