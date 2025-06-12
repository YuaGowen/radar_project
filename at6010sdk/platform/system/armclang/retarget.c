/**
 * Copyright (c) 2023, Airtouching Intelligence Technology.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Airtouching
 *    Intelligence Technology integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY AIRTOUCHING TECHNOLOGY "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AIRTOUCHING TECHNOLOGY OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


/******************************************************************************/
/* Retarget functions for ARM DS-5 Professional / Keil MDK                    */
/******************************************************************************/

#include <stdio.h>
#include <rt_misc.h>
#include <rt_sys.h>
#include <time.h>

#ifdef CORTEX_M0PLUS
#include "CMSDK_CM0plus.h"
#endif

#include "jiffies.h"
#include "uart.h"
#include "common.h"
#include "hal_dualtimer.h"

__asm(".global __use_no_semihosting");

#ifndef UART_LOG_PORT
#define DEFAULT_UART_CH                 UART_ID_0
#else
#define DEFAULT_UART_CH                 UART_LOG_PORT
#endif
#define DEFAULT_HANDLE                  0x100
#define DELAY_LOOP_CYCLES               5U

FILE __stdout;
FILE __stdin;
FILE __stderr;

/* Low-Level I/O Implementations ---------------------------------------------*/

int fputc(int ch, FILE *f)
{
	(void)f;

	return (int)(uart_send_byte(DEFAULT_UART_CH, (u8)ch));
}

int fgetc(FILE *f)
{
	(void)f;

	return (uart_send_byte(DEFAULT_UART_CH, uart_recv_byte(DEFAULT_UART_CH)));
}

int ferror(FILE *f)
{
	(void)f;

	/* Your implementation of ferror */
	return EOF;
}

/* System I/O Implementations ------------------------------------------------*/

/*
 * Open a file. May return -1 if the file failed to open. We do not require
 * this function to do anything. Simply return a dummy handle.
 */
FILEHANDLE _sys_open(const char *name, int openmode)
{
	(void)name;
	(void)openmode;

	return DEFAULT_HANDLE;
}

/*
 * Close a file. Should return 0 on success or a negative value on error.
 * Not required in this implementation. Always return success.
 */
int _sys_close(FILEHANDLE fh)
{
	(void)fh;

	return 0;
}

/*
 * Write to a file. Returns 0 on success, negative on error, and the number
 * of characters _not_ written on partial success. This implementation sends
 * a buffer of size 'len' to the UART.
 */
int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)
{
	(void)fh;
	(void)buf;
	(void)len;
	(void)mode;

	return 0;
}

/*
 * Read from a file. Can return:
 *  - zero if the read was completely successful
 *  - the number of bytes _not_ read, if the read was partially successful
 *  - the number of bytes not read, plus the top bit set (0x80000000), if
 *    the read was partially successful due to end of file
 *  - -1 if some error other than EOF occurred
 * This function receives a character from the UART, processes the character
 * if required (backspace) and then echo the character to the Terminal
 * Emulator, printing the correct sequence after successive keystrokes.
 */
int _sys_read(FILEHANDLE fh, unsigned char *buf, unsigned len, int mode)
{
	(void)fh;
	(void)buf;
	(void)len;
	(void)mode;

	return 0;
}

/**
 * Writes a character to the output channel. This function is used
 * for last-resort error message output.
 */
void _ttywrch(int ch)
{
	uart_send_byte(DEFAULT_UART_CH, (u8)ch);
}

/*
 * Return non-zero if the argument file is connected to a terminal.
 */
int _sys_istty(FILEHANDLE fh)
{
	(void)fh;

	return 1;   // no interactive device present
}

/*
 * Move the file position to a given offset from the file start.
 * Returns >=0 on success, <0 on failure. Seeking is not supported for the
 * UART.
 */
int _sys_seek(FILEHANDLE fh, long pos)
{
	(void)fh;
	(void)pos;

	return -1;   // error
}

/*
 * Flush any OS buffers associated with fh, ensuring that the file
 * is up to date on disk. Result is >=0 if OK, negative for an
 * error.
 */
int _sys_ensure(FILEHANDLE fh)
{
	(void)fh;

	return 0;   // success
}

/*
 * Return the current length of a file, or <0 if an error occurred.
 * _sys_flen is allowed to reposition the file pointer (so Unix can
 * implement it with a single lseek, for example), since it is only
 * called when processing SEEK_END relative fseeks, and therefore a
 * call to _sys_flen is always followed by a call to _sys_seek.
 */
long _sys_flen(FILEHANDLE fh)
{
	(void)fh;

	return 0;
}


#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6190000)
/*
 * Return the name for temporary file number sig in the buffer
 * name. maxlen is the maximum name length allowed.
 *
 * Return value is 0 on success, or negative on failure (similarly to
 * Unix system calls, and also matching the SYS_TMPNAM semihosting
 * call). If this function returns failure, it need not write anything
 * into the output buffer at all.
 */
int _sys_tmpnam2(char *name, int sig, unsigned maxlen)
{
	return 0;
}

/*
 * Legacy variant of _sys_tmpnam2, which returns void and has no way
 * to report failure. If you implement this, the library will provide
 * a _sys_tmpnam2 wrapper that calls it.
 */
void _sys_tmpnam(char * name, int sig, unsigned maxlen)
{

}

#else

/*
 * Return the name for temporary file number sig in the buffer
 * name. Returns 0 on failure. maxlen is the maximum name length
 * allowed.
 */
int _sys_tmpnam(char *name, int sig, unsigned maxlen)
{
	(void)name;
	(void)sig;
	(void)maxlen;

	return 0;
}

#endif

/*
 * Terminate the program, passing a return code back to the user.
 * This function may not return.
 */
__NO_RETURN void _sys_exit(int return_code)
{
	(void)return_code;

	for (;;) {}
}

/*
 * determines the current calendar time. The encoding of the value is
 * unspecified.
 * Returns: the implementations best approximation to the current calendar
 *          time. The value (time_t)-1 is returned if the calendar time is
 *          not available. If timer is not a null pointer, the return value
 *          is also assigned to the object it points to.*/

time_t time(time_t *p)
{
	(void)p;

	return 0;
}

/* Delay us/ms Implementations -----------------------------------------------*/

void delay_us(u32 us)
{
	hal_dualtimer_delay_us(us);
}

void delay_ms(u32 ms)
{
	hal_dualtimer_delay_ms(ms);
}
