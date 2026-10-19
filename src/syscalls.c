/*
 * syscalls.c
 *
 *  Created on: 2026年10月17日
 *      Author: r1072
 */
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "uart_debug.h"

int _write(int file, char *ptr, int len)
{
    (void) file;

    if ((ptr == NULL) || (len <= 0))
    {
        return 0;
    }

    if (uart_debug_write(
            (const uint8_t *) ptr,
            (uint32_t) len
        ) != FSP_SUCCESS)
    {
        return -1;
    }

    return len;
}

int _read(int file, char *ptr, int len)
{
    (void) file;
    (void) ptr;
    (void) len;

    return 0;
}

int _close(int file)
{
    (void) file;
    return -1;
}

int _fstat(int file, struct stat *st)
{
    (void) file;

    st->st_mode = S_IFCHR;

    return 0;
}

int _isatty(int file)
{
    (void) file;
    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    (void) file;
    (void) ptr;
    (void) dir;

    return 0;
}

int _getpid(void)
{
    return 1;
}

int _kill(int pid, int sig)
{
    (void) pid;
    (void) sig;

    errno = EINTR;

    return -1;
}

