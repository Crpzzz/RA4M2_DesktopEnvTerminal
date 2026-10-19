/*
 * uart_debug.h
 *
 *  Created on: 2026年10月17日
 *      Author: r1072
 */

#ifndef UART_DEBUG_H_
#define UART_DEBUG_H_

#include "hal_data.h"
#include <stdint.h>

fsp_err_t uart_debug_init(void);

fsp_err_t uart_debug_write(
    const uint8_t *data,
    uint32_t length
);

#endif /* UART_DEBUG_H_ */
