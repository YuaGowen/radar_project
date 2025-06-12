/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*
        BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR UART0.
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* Library includes. */
#include "hal_uart.h"
#include "at6010.h"

/* Demo application includes. */
#include "serial.h"

/*-----------------------------------------------------------*/
#define SERIAL_ECHO_ENABLE
#define CLI_SERIAL_PORT  UART_LOG_PORT

/* Misc defines. */
#define serINVALID_QUEUE ((QueueHandle_t)0)
#define serNO_BLOCK      ((TickType_t)0)
#define serTX_BLOCK_TIME (40 / portTICK_PERIOD_MS)

/*-----------------------------------------------------------*/

/* The queue used to hold received characters. */
static QueueHandle_t xRxedChars;
static QueueHandle_t xCharsForTx;

/*-----------------------------------------------------------*/

/* UART interrupt handler. */
static void uart_rx_handler(void);
static void uart_tx_empty_handler(void);

/*-----------------------------------------------------------*/

static void uart_cli_port_init(uart_id_t id, u32 baudrate)
{
	uart_cfg_t uart_cfg;

	uart_cfg.baud_rate = baudrate;
	uart_cfg.parity = UART_PARITY_DISABLE;
	uart_cfg.stop_bits = UART_STOP_BITS_1;
	uart_cfg.data_width = UART_DATA_WIDTH_8BIT;
	uart_cfg.fifo_cfg.fifo_en = 1U;
	uart_cfg.fifo_cfg.rx_fifo_level = UART_RX_FIFO_CHAR_1;
	uart_cfg.fifo_cfg.tx_fifo_level = UART_TX_FIFO_EMPTY;
	uart_cfg.irq_enable = 0U;

	while (0U == uart_get_status(id, UART_TX_BUF_EMPTY)) {} /* Wait uart tx buffer empty */
	while (0U != uart_get_status(id, UART_BUSY)) {}         /* Wait uart idle */
	(void)hal_uart_init(id, &uart_cfg);

	uart_set_hdl(id, UART_TX_EMPTY_INTR, uart_tx_empty_handler);
	uart_set_hdl(id, UART_RX_INTR, uart_rx_handler);
	uart_irq_enable(id, UART_RX_INTR);

	NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + id));
}

/*
 * See the serial.h header file.
 */
xComPortHandle xSerialPortInitMinimal(unsigned long ulWantedBaud,
                                      unsigned portBASE_TYPE uxQueueLength)
{
	xComPortHandle xReturn = NULL;

	/* Create the queues used to hold Rx/Tx characters. */
	xRxedChars = xQueueCreate(uxQueueLength, (unsigned portBASE_TYPE)sizeof(signed char));
	xCharsForTx = xQueueCreate(uxQueueLength + 1, (unsigned portBASE_TYPE)sizeof(signed char));

	/* If the queue/semaphore was created correctly then setup the serial port
	hardware. */
	if ((xRxedChars != serINVALID_QUEUE) && (xCharsForTx != serINVALID_QUEUE)) {
		uart_cli_port_init(CLI_SERIAL_PORT, ulWantedBaud);
	} else {
		xReturn = (xComPortHandle)0;
	}

	/* This demo file only supports a single port but we have to return
	something to comply with the standard demo header file. */
	return xReturn;
}

/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar(xComPortHandle pxPort, signed char *pcRxedChar,
                                    TickType_t xBlockTime)
{
	/* The port handle is not required as this driver only supports one port. */
	(void)pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if (xQueueReceive(xRxedChars, pcRxedChar, xBlockTime)) {
#ifdef SERIAL_ECHO_ENABLE
		while (pdPASS != xSerialPutChar(pxPort, *pcRxedChar, serNO_BLOCK)) {}
#endif
		return pdTRUE;
	} else {
		return pdFALSE;
	}
}

/*-----------------------------------------------------------*/

void vSerialPutString(xComPortHandle pxPort, const signed char *const pcString,
                      unsigned short usStringLength)
{
	signed char *pxNext;

	/* A couple of parameters that this port does not use. */
	(void)usStringLength;

	/* NOTE: This implementation does not handle the queue being full as no
	block time is used! */

	/* Send each character in the string, one at a time. */
	pxNext = (signed char *)pcString;
	while (*pxNext) {
		xSerialPutChar(pxPort, *pxNext, serNO_BLOCK);
		pxNext++;
	}
}

/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar(xComPortHandle pxPort, signed char cOutChar,
                                    TickType_t xBlockTime)
{
	signed portBASE_TYPE xReturn;

	if (xQueueSend(xCharsForTx, &cOutChar, xBlockTime) == pdPASS) {
		xReturn = pdPASS;
		uart_irq_enable(CLI_SERIAL_PORT, UART_TX_EMPTY_INTR);
	} else {
		xReturn = pdFAIL;
	}

	return xReturn;
}

/*-----------------------------------------------------------*/

void vSerialClose(xComPortHandle xPort)
{
	/* Not supported as not required by the demo application. */
}

/*-----------------------------------------------------------*/

static void uart_rx_handler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	u8 byte;

	// uart_irq_clear(CLI_SERIAL_PORT, UART_RX_INTR);

	while (0U == uart_get_status(CLI_SERIAL_PORT, UART_RX_BUF_EMPTY)) {
		byte = uart_recv_byte(CLI_SERIAL_PORT);
		xQueueSendFromISR(xRxedChars, &byte, &xHigherPriorityTaskWoken);
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

static void uart_tx_empty_handler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	char cChar;

	// uart_irq_clear(CLI_SERIAL_PORT, UART_TX_EMPTY_INTR);

	/* The interrupt was caused by the THR becoming empty.  Are there any
	more characters to transmit? */
	if (xQueueReceiveFromISR(xCharsForTx, &cChar, &xHigherPriorityTaskWoken) == pdTRUE) {
		/* A character was retrieved from the queue so can be sent to the
		THR now. */
		(void)uart_send_byte(CLI_SERIAL_PORT, cChar);
	} else {
		uart_irq_disable(CLI_SERIAL_PORT, UART_TX_EMPTY_INTR);
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
