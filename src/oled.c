#include "oled.h"
#include "oled_font.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define OLED_I2C_ADDRESS    (0x3CU)
#define OLED_WIDTH          (128U)
#define OLED_PAGE_COUNT     (8U)

static volatile bool s_i2c_tx_done = false;

void oled_i2c_callback(i2c_master_callback_args_t *p_args)
{
    if (p_args == NULL)
    {
        return;
    }

    if (p_args->event == I2C_MASTER_EVENT_TX_COMPLETE)
    {
        s_i2c_tx_done = true;
    }
}

static fsp_err_t oled_i2c_write(
    uint8_t *data,
    uint32_t length
)
{
    fsp_err_t error;
    uint32_t timeout_count = 0U;

    if ((data == NULL) || (length == 0U))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    s_i2c_tx_done = false;

    error = R_IIC_MASTER_Write(
        &g_i2c_master0_ctrl,
        data,
        length,
        false
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    while ((!s_i2c_tx_done) &&
           (timeout_count < 2000U))
    {
        R_BSP_SoftwareDelay(
            100U,
            BSP_DELAY_UNITS_MICROSECONDS
        );

        timeout_count++;
    }

    if (!s_i2c_tx_done)
    {
        R_IIC_MASTER_Abort(
            &g_i2c_master0_ctrl
        );

        return FSP_ERR_TIMEOUT;
    }

    return FSP_SUCCESS;
}

static fsp_err_t oled_write_command(
    uint8_t command
)
{
    uint8_t buffer[2];

    /*
     * 0x00 表示后面的字节是命令。
     */
    buffer[0] = 0x00U;
    buffer[1] = command;

    return oled_i2c_write(
        buffer,
        2U
    );
}

static fsp_err_t oled_write_data(
    uint8_t *data,
    uint32_t length
)
{
    uint8_t buffer[129];

    if ((data == NULL) || (length == 0U))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    if (length > OLED_WIDTH)
    {
        return FSP_ERR_INVALID_SIZE;
    }

    /*
     * 0x40 表示后面的字节是显示数据。
     */
    buffer[0] = 0x40U;

    for (uint32_t i = 0U; i < length; i++)
    {
        buffer[i + 1U] = data[i];
    }

    return oled_i2c_write(
        buffer,
        length + 1U
    );
}

static void oled_delay_10ms(void)
{
    R_BSP_SoftwareDelay(
        10U,
        BSP_DELAY_UNITS_MILLISECONDS
    );
}

fsp_err_t oled_init(void)
{
    fsp_err_t error;

    /*
     * 打开硬件 IIC0。
     */
    error = R_IIC_MASTER_Open(
        &g_i2c_master0_ctrl,
        &g_i2c_master0_cfg
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    /*
     * OLED 常见地址为 0x3C。
     */
    error = R_IIC_MASTER_SlaveAddressSet(
        &g_i2c_master0_ctrl,
        OLED_I2C_ADDRESS,
        I2C_MASTER_ADDR_MODE_7BIT
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    oled_delay_10ms();

    /*
     * SSD1306/SSD1312 常用初始化命令。
     */
    static uint8_t init_commands[] =
    {
        0xAEU,
        0xD5U, 0x80U,
        0xA8U, 0x3FU,
        0xD3U, 0x00U,
        0x40U,
        0x8DU, 0x14U,
        0x20U, 0x00U,

        /*
         * 0xA0：左右正常显示
         */
        0xA0U,

        /*
         * 0xC8：上下方向保持当前配置
         */
        0xC8U,

        0xDAU, 0x12U,
        0x81U, 0x8FU,
        0xD9U, 0xF1U,
        0xDBU, 0x40U,
        0xA4U,
        0xA6U,
        0xAFU
    };

    for (uint32_t i = 0U;
         i < sizeof(init_commands);
         i++)
    {
        error = oled_write_command(
            init_commands[i]
        );

        if (error != FSP_SUCCESS)
        {
            return error;
        }
    }

    return oled_clear();
}

fsp_err_t oled_clear(void)
{
    uint8_t empty_line[OLED_WIDTH] = {0};

    for (uint8_t page = 0U;
         page < OLED_PAGE_COUNT;
         page++)
    {
        if (oled_write_command(
                (uint8_t)(0xB0U + page)
            ) != FSP_SUCCESS)
        {
            return FSP_ERR_ABORTED;
        }

        if (oled_write_command(0x00U) != FSP_SUCCESS)
        {
            return FSP_ERR_ABORTED;
        }

        if (oled_write_command(0x10U) != FSP_SUCCESS)
        {
            return FSP_ERR_ABORTED;
        }

        fsp_err_t error = oled_write_data(
            empty_line,
            OLED_WIDTH
        );

        if (error != FSP_SUCCESS)
        {
            return error;
        }
    }

    return FSP_SUCCESS;
}

static void oled_set_position(
    uint8_t page,
    uint8_t column
)
{
    oled_write_command(
        (uint8_t)(0xB0U + page)
    );

    oled_write_command(
        (uint8_t)(column & 0x0FU)
    );

    oled_write_command(
        (uint8_t)(0x10U | (column >> 4U))
    );
}

static fsp_err_t oled_show_string(
    uint8_t page,
    uint8_t column,
    const char *text
)
{
    uint8_t glyph[5];

    if (text == NULL)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    oled_set_position(
        page,
        column
    );

    while (*text != '\0')
    {
        if (!oled_font_get(
                *text,
                glyph
            ))
        {
            /*
             * 不支持的字符显示为空格。
             */
            glyph[0] = 0x00U;
            glyph[1] = 0x00U;
            glyph[2] = 0x00U;
            glyph[3] = 0x00U;
            glyph[4] = 0x00U;
        }

        fsp_err_t error = oled_write_data(
            glyph,
            5U
        );

        if (error != FSP_SUCCESS)
        {
            return error;
        }

        /*
         * 字符之间空一列。
         */
        uint8_t blank = 0x00U;

        error = oled_write_data(
            &blank,
            1U
        );

        if (error != FSP_SUCCESS)
        {
            return error;
        }

        text++;
    }

    return FSP_SUCCESS;
}
/*
 * 更新 OLED 上的光照 ADC 数值。
 * 使用 page 2，也就是屏幕从上往下的第三个文字行。
 */
fsp_err_t oled_show_light_adc(uint16_t adc_value)
{
    char text[20];
    uint8_t empty_line[128] = {0};

    /*
     * 先清除这一行，避免新数字比旧数字短时留下残影。
     */
    fsp_err_t error = oled_write_command(0xB2U);

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    error = oled_write_command(0x00U);

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    error = oled_write_command(0x10U);

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    error = oled_write_data(
        empty_line,
        sizeof(empty_line)
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    /*
     * 把 ADC 数字值转换成字符串。
     * 例如：LIGHT: 1234
     */
    (void) snprintf(
        text,
        sizeof(text),
        "LIGHT: %u",
        (unsigned int) adc_value
    );

    /*
     * 在 page 2、column 0 显示字符串。
     */
    return oled_show_string(
        2U,
        0U,
        text
    );
}
/*
 * 清除指定文字行，再显示新的字符串。
 * page 的范围是 0 到 7。
 */
static fsp_err_t oled_show_line(
    uint8_t page,
    const char *text
)
{
    uint8_t empty_line[128] = {0};
    fsp_err_t error;

    if (page > 7U)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    error = oled_write_command(
        (uint8_t)(0xB0U + page)
    );
    if (error != FSP_SUCCESS)
    {
        return error;
    }

    error = oled_write_command(0x00U);
    if (error != FSP_SUCCESS)
    {
        return error;
    }

    error = oled_write_command(0x10U);
    if (error != FSP_SUCCESS)
    {
        return error;
    }

    error = oled_write_data(
        empty_line,
        sizeof(empty_line)
    );
    if (error != FSP_SUCCESS)
    {
        return error;
    }

    return oled_show_string(
        page,
        0U,
        text
    );
}
fsp_err_t oled_show_control(
    uint16_t light_adc,
    uint8_t pwm_percent,
    bool auto_mode
)
{
    char line[22];
    fsp_err_t error;

    /*
     * 第 0 行：项目名称。
     */
    error = oled_show_line(
        0U,
        "SMART DESK"
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    /*
     * 第 2 行：光照 ADC 值。
     */
    (void) snprintf(
        line,
        sizeof(line),
        "LIGHT: %u",
        (unsigned int) light_adc
    );

    error = oled_show_line(
        2U,
        line
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    /*
     * 第 4 行：PWM 占空比。
     */
    (void) snprintf(
        line,
        sizeof(line),
        "PWM: %u %%",
        (unsigned int) pwm_percent
    );

    error = oled_show_line(
        4U,
        line
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    /*
     * 第 6 行：工作模式。
     */
    if (auto_mode)
    {
        return oled_show_line(
            6U,
            "MODE: AUTO"
        );
    }

    return oled_show_line(
        6U,
        "MODE: MANUAL"
    );
}

/*
 * 更新桌面环境信息。
 */
fsp_err_t oled_show_environment(
    bool dht_valid,
    uint8_t temperature,
    uint8_t humidity,
    uint16_t light_adc
)
{
    char line[22];
    fsp_err_t error;

    /*
     * 第 0 行：项目名称
     */
    error = oled_show_line(
        0U,
        "SMART DESK"
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    /*
     * 第 2 行：温度。
     * 传感器没到时显示占位符。
     */
    if (dht_valid)
    {
        (void) snprintf(
            line,
            sizeof(line),
            "TEMP: %u C",
            (unsigned int) temperature
        );
    }
    else
    {
        (void) snprintf(
            line,
            sizeof(line),
            "TEMP: -- C"
        );
    }

    error = oled_show_line(
        2U,
        line
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    /*
     * 第 4 行：湿度。
     */
    if (dht_valid)
    {
        (void) snprintf(
            line,
            sizeof(line),
            "HUMI: %u %%",
            (unsigned int) humidity
        );
    }
    else
    {
        (void) snprintf(
            line,
            sizeof(line),
            "HUMI: -- %%"
        );
    }

    error = oled_show_line(
        4U,
        line
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    /*
     * 第 6 行：光照 ADC 数值。
     */
    (void) snprintf(
        line,
        sizeof(line),
        "LIGHT: %u",
        (unsigned int) light_adc
    );

    return oled_show_line(
        6U,
        line
    );
}

fsp_err_t oled_show_startup(void)
{
    fsp_err_t error;

    error = oled_clear();

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    error = oled_show_string(
        1U,
        20U,
        "SMART DESK"
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    error = oled_show_string(
        3U,
        20U,
        "UART OK"
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    return oled_show_string(
        5U,
        20U,
        "GPT OK"
    );
}
fsp_err_t oled_show_control_percent(
    uint8_t light_percent,
    uint8_t target_percent,
    uint8_t pwm_percent,
    bool auto_mode
)
{
    char line[22];
    fsp_err_t error;

    error = oled_show_line(
        0U,
        "SMART DESK"
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    (void) snprintf(
        line,
        sizeof(line),
        "LIGHT: %u %%",
        (unsigned int) light_percent
    );

    error = oled_show_line(2U, line);

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    (void) snprintf(
        line,
        sizeof(line),
        "TARGET: %u %%",
        (unsigned int) target_percent
    );

    error = oled_show_line(4U, line);

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    (void) snprintf(
        line,
        sizeof(line),
        "PWM: %u %%",
        (unsigned int) pwm_percent
    );

    error = oled_show_line(6U, line);

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    /*
     * 当前页面显示四行，
     * 模式暂时通过串口输出。
     */
    (void) auto_mode;

    return FSP_SUCCESS;
}

fsp_err_t oled_show_status_percent(
    uint8_t light_percent,
    uint8_t target_percent,
    uint8_t pwm_percent,
    bool auto_mode,
    bool dht_valid,
    uint8_t temperature,
    uint8_t humidity
)
{
    char line[22];
    fsp_err_t error;

    /* 第 0 行显示当前工作模式。 */
    (void) snprintf(
        line,
        sizeof(line),
        "MODE:%s",
        auto_mode ? "AUTO" : "MANUAL"
    );

    error = oled_show_line(0U, line);
    if (error != FSP_SUCCESS) return error;

    /* 第 2 行显示当前环境光照百分比。 */
    (void) snprintf(
        line,
        sizeof(line),
        "LIGHT:%u%%",
        (unsigned int) light_percent
    );

    error = oled_show_line(2U, line);
    if (error != FSP_SUCCESS) return error;

    /* 第 4 行显示温度和湿度。 */
    if (dht_valid)
    {
        (void) snprintf(
            line,
            sizeof(line),
            "TEMP:%uC H:%u%%",
            (unsigned int) temperature,
            (unsigned int) humidity
        );
    }
    else
    {
        (void) snprintf(
            line,
            sizeof(line),
            "TEMP:--C H:--%%"
        );
    }

    error = oled_show_line(4U, line);
    if (error != FSP_SUCCESS) return error;

    /* 第 6 行显示 PWM 输出和目标亮度。 */
    (void) snprintf(
        line,
        sizeof(line),
        "PWM:%u%% SET:%u%%",
        (unsigned int) pwm_percent,
        (unsigned int) target_percent
    );

    return oled_show_line(6U, line);
}
