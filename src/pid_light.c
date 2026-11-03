/*
 * pid_light.c
 *
 *  Created on: 2026年10月20日
 *      Author: r1072
 */
#include "pid_light.h"

/*
 * 把数值限制在 low 到 high 之间。
 */
static float limit(
    float value,
    float low,
    float high
)
{
    if (value < low)
    {
        return low;
    }

    if (value > high)
    {
        return high;
    }

    return value;
}

/*
 * 进入自动模式时初始化控制器。
 */
void pid_light_start(
    pid_light_t *pid,
    float target,
    float measured,
    float output
)
{
    /*
     * 起步参数，后面根据实际灯具调整。
     */
    pid->kp = 0.5f;
    pid->ki = 0.3f;

    /*
     * 从切换前的 PWM 开始控制。
     */
    pid->output = limit(
        output,
        0.0f,
        100.0f
    );

    /*
     * PI 输出 = 比例项 + 积分项。
     *
     * 反过来设置积分项，使初始 PI 输出等于当前 PWM，
     * 避免切到自动模式时突然从零重新开始。
     */
    pid->integral =
        pid->output -
        pid->kp * (target - measured);
}

/*
 * 执行一次 PI 运算。
 */
float pid_light_update(
    pid_light_t *pid,
    float target,
    float measured,
    float dt
)
{
    /*
     * 正误差：实际光照不足。
     * 负误差：实际光照过高。
     */
    float error = target - measured;

    /*
     * 调度间隔异常时，保持本次输出，
     * 避免把很长一段时间的误差一次累加进去。
     */
    if ((dt <= 0.0f) || (dt > 0.5f))
    {
        pid->integral =
            pid->output - pid->kp * error;

        return pid->output;
    }

    /*
     * 误差在正负 1 个百分点内时，保持输出，
     * 减少整数光照读数引起的反复微调。
     */
    if ((error >= -1.0f) && (error <= 1.0f))
    {
        pid->integral =
            pid->output - pid->kp * error;

        return pid->output;
    }

    /*
     * 比例项：根据当前误差进行调节。
     */
    float proportional = pid->kp * error;

    /*
     * 积分项：误差乘以经过的秒数，再逐次累加。
     */
    float integral =
        pid->integral + pid->ki * error * dt;

    float requested = proportional + integral;

    /*
     * 每秒最多变化 30 个百分点。
     * 如果 dt 为 0.1 秒，本次最多变化 3 个百分点。
     */
    float step = 30.0f * dt;

    float output = limit(
        requested,
        pid->output - step,
        pid->output + step
    );

    /*
     * 最终 PWM 必须位于 0~100。
     */
    output = limit(
        output,
        0.0f,
        100.0f
    );

    /*
     * 积分跟踪实际受限的输出。
     *
     * 防止输出已经到上限、下限或变化速度受到限制时，
     * 积分还在继续无限累加。
     */
    pid->integral = output - proportional;

    pid->output = output;

    return output;
}

