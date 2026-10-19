/*
 * uart_debug.c
 *
 *  Created on: 2026年10月17日
 *      Author: r1072
 */
#include "uart_debug.h"

#include <stdbool.h>

static volatile bool s_tx_done = false;

void uart9_callback(uart_callback_args_t *p_args)
{
    if ((p_args != NULL) &&
        (p_args->event == UART_EVENT_TX_COMPLETE))
    {
        s_tx_done = true;
    }
}

fsp_err_t uart_debug_init(void)
{
    return R_SCI_UART_Open(
        &g_uart9_ctrl,
        &g_uart9_cfg
    );
}

fsp_err_t uart_debug_write(
    const uint8_t *data,
    uint32_t length
)
{
    fsp_err_t error;
    uint32_t count = 0U;

    if ((data == NULL) || (length == 0U))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    s_tx_done = false;

    error = R_SCI_UART_Write(
        &g_uart9_ctrl,
        data,
        length
    );

    if (error != FSP_SUCCESS)
    {
        return error;
    }

    while ((!s_tx_done) && (count < 2000U))
    {
        R_BSP_SoftwareDelay(
            100U,
            BSP_DELAY_UNITS_MICROSECONDS
        );

        count++;
    }

    if (!s_tx_done)
    {
        return FSP_ERR_TIMEOUT;
    }

    return FSP_SUCCESS;
}

