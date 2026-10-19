/*
 * dht11.c
 *
 *  Created on: 2026年10月18日
 *      Author: r1072
 */
#include "dht11.h"

#include <stdbool.h>
#include <stddef.h>

/*
 * 工程之前使用的是 st_ioport_ctrl。
 * 如果 LED.c 中实际使用的是 g_ioport_ctrl，
 * 就把下面这一行改成 g_ioport_ctrl。
 */
#define DHT11_IOPORT_CTRL    g_ioport_ctrl

/*
 * P002 = Port 0, Pin 2。
 */
#define DHT11_DATA_PIN       BSP_IO_PORT_00_PIN_02

#define DHT11_TIMEOUT_US     (100U)

/*
 * 读取 P002 当前电平。
 */
static bsp_io_level_t dht11_pin_read(void)
{
    bsp_io_level_t level = BSP_IO_LEVEL_LOW;

    R_IOPORT_PinRead(
        &DHT11_IOPORT_CTRL,
        DHT11_DATA_PIN,
        &level
    );

    return level;
}

/*
 * 等待数据线变成指定电平。
 *
 * expected_level:
 *     BSP_IO_LEVEL_LOW
 *     BSP_IO_LEVEL_HIGH
 *
 * timeout_us:
 *     最大等待时间，单位为微秒。
 */
static bool dht11_wait_level(
    bsp_io_level_t expected_level,
    uint32_t timeout_us
)
{
    while (timeout_us > 0U)
    {
        if (dht11_pin_read() == expected_level)
        {
            return true;
        }

        R_BSP_SoftwareDelay(
            1U,
            BSP_DELAY_UNITS_MICROSECONDS
        );

        timeout_us--;
    }

    return false;
}

/*
 * 将 P002 设置为输出，并输出低电平。
 */
static void dht11_set_output_low(void)
{
    /*
     * 将 P002 配置为 GPIO 输出，并输出低电平。
     */
    R_IOPORT_PinCfg(
        &DHT11_IOPORT_CTRL,
        DHT11_DATA_PIN,
        IOPORT_CFG_PORT_DIRECTION_OUTPUT |
        IOPORT_CFG_PORT_OUTPUT_LOW
    );
}

/*
 * 释放 P002，让 DHT11 控制数据线。
 */
static void dht11_release_bus(void)
{
    /*
     * 把 P002 改成输入。
     *
     * DHT11 通过外部上拉电阻把数据线拉高，
     * 然后由 DHT11 主动拉低或释放数据线。
     */
    R_IOPORT_PinCfg(
        &DHT11_IOPORT_CTRL,
        DHT11_DATA_PIN,
        IOPORT_CFG_PORT_DIRECTION_INPUT |
        IOPORT_CFG_PIM_TTL
    );
}

/*
 * 读取一个数据位。
 */
static fsp_err_t dht11_read_bit(
    uint8_t *bit
)
{
    if (bit == NULL)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /*
     * 每一位开始时，DHT11 会先拉低约 50 us。
     */
    if (!dht11_wait_level(
            BSP_IO_LEVEL_LOW,
            DHT11_TIMEOUT_US
        ))
    {
        return FSP_ERR_TIMEOUT;
    }

    /*
     * 随后 DHT11 把数据线拉高。
     */
    if (!dht11_wait_level(
            BSP_IO_LEVEL_HIGH,
            DHT11_TIMEOUT_US
        ))
    {
        return FSP_ERR_TIMEOUT;
    }

    /*
     * 高电平持续时间决定数据是 0 还是 1：
     *
     * 0：大约 26~28 us
     * 1：大约 70 us
     *
     * 等待 40 us 后采样：
     * - 如果已经变低，就是 0
     * - 如果仍然为高，就是 1
     */
    R_BSP_SoftwareDelay(
        40U,
        BSP_DELAY_UNITS_MICROSECONDS
    );

    if (dht11_pin_read() == BSP_IO_LEVEL_HIGH)
    {
        *bit = 1U;
    }
    else
    {
        *bit = 0U;
    }

    /*
     * 等待当前位结束，回到低电平。
     */
    if (!dht11_wait_level(
            BSP_IO_LEVEL_LOW,
            DHT11_TIMEOUT_US
        ))
    {
        return FSP_ERR_TIMEOUT;
    }

    return FSP_SUCCESS;
}

/*
 * 初始化 DHT11 数据线。
 */
fsp_err_t dht11_init(void)
{
    /*
     * 初始状态：
     * P002 是输出，并保持高电平。
     */
    fsp_err_t error = R_IOPORT_PinCfg(
        &DHT11_IOPORT_CTRL,
        DHT11_DATA_PIN,
        IOPORT_CFG_PORT_DIRECTION_OUTPUT |
        IOPORT_CFG_PORT_OUTPUT_HIGH
    );

    return error;
}

/*
 * 读取 DHT11 温湿度。
 */
fsp_err_t dht11_read(
    dht11_data_t *data
)
{
    uint8_t raw_data[5] = {0U};
    fsp_err_t error;

    if (data == NULL)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /*
     * 第一步：
     * MCU 把数据线拉低至少 18 ms。
     */
    dht11_set_output_low();

    R_BSP_SoftwareDelay(
        20U,
        BSP_DELAY_UNITS_MILLISECONDS
    );

    /*
     * 第二步：
     * MCU 释放数据线，等待 DHT11 响应。
     */
    dht11_release_bus();

    R_BSP_SoftwareDelay(
        30U,
        BSP_DELAY_UNITS_MICROSECONDS
    );

    /*
     * 第三步：
     * DHT11 响应：
     * 低电平约 80 us，
     * 高电平约 80 us，
     * 然后开始发送数据。
     */
    if (!dht11_wait_level(
            BSP_IO_LEVEL_LOW,
            DHT11_TIMEOUT_US
        ))
    {
        return FSP_ERR_TIMEOUT;
    }

    if (!dht11_wait_level(
            BSP_IO_LEVEL_HIGH,
            DHT11_TIMEOUT_US
        ))
    {
        return FSP_ERR_TIMEOUT;
    }

    if (!dht11_wait_level(
            BSP_IO_LEVEL_LOW,
            DHT11_TIMEOUT_US
        ))
    {
        return FSP_ERR_TIMEOUT;
    }

    /*
     * 第四步：
     * 读取 40 位数据：
     *
     * raw_data[0]：湿度整数部分
     * raw_data[1]：湿度小数部分
     * raw_data[2]：温度整数部分
     * raw_data[3]：温度小数部分
     * raw_data[4]：校验和
     */
    for (uint32_t byte_index = 0U;
         byte_index < 5U;
         byte_index++)
    {
        for (uint32_t bit_index = 0U;
             bit_index < 8U;
             bit_index++)
        {
            uint8_t bit_value = 0U;

            error = dht11_read_bit(
                &bit_value
            );

            if (error != FSP_SUCCESS)
            {
                return error;
            }

            raw_data[byte_index] <<= 1U;
            raw_data[byte_index] |= bit_value;
        }
    }

    /*
     * 第五步：
     * 校验前四个字节的和。
     */
    uint8_t checksum =
        (uint8_t)(
            raw_data[0] +
            raw_data[1] +
            raw_data[2] +
            raw_data[3]
        );

    if (checksum != raw_data[4])
    {
        return FSP_ERR_INVALID_DATA;
    }

    /*
     * DHT11 通常输出整数温度和整数湿度。
     */
    data->humidity = raw_data[0];
    data->temperature = raw_data[2];

    return FSP_SUCCESS;
}

