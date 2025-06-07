/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * NOTE:  This file uses a third party USB CDC driver.
 */

/* CLI includes. */
#include "FreeRTOS_CLI.h"
#include "serial.h"
#include "version.h"

/*
 * The maximum time to wait for the mutex that guards the UART to become
 * available.
 */
#define cmdMAX_MUTEX_WAIT               pdMS_TO_TICKS( 300 )
#define SERIAL_BAUDRATE                 UART_LOG_BAUDRATE
#define SERIAL_QUEUE_SIZE               128U

#define CFG_BOARD_NAME                  FIRMWARE_NAME
/*-----------------------------------------------------------*/

/*
 * The task that implements the command console processing.
 */
static void prvUARTCommandConsoleTask( void *pvParameters );
void vUARTCommandConsoleStart( uint16_t usStackSize, UBaseType_t uxPriority );

/*-----------------------------------------------------------*/

/* Const messages output by the command console. */
static const char * const pcEndOfOutputMessage = " > ";
static const char * const pcNewLine = "\r\n";

/*
 * g_cmd_history[][]             Circular buffer
 * g_cmd_history_head            Head of the circular buffer, most recent command
 * g_cmd_history_steps_from_head Offset from head
 * g_cmd_history_len             Number of elements in the circular buffer
 */

static char g_cmd_history[cmdMAX_CMD_CNT][cmdMAX_INPUT_SIZE];
static int g_cmd_history_head = -1;
static int g_cmd_history_steps_from_head = 1;
static int g_cmd_history_len = 0;

/*
 * Used to guard access to the UART in case messages are sent to the UART from
 * more than one task.
 */
static SemaphoreHandle_t xTxMutex = NULL;

/*-----------------------------------------------------------*/

void vUARTCommandConsoleStart( uint16_t usStackSize, UBaseType_t uxPriority )
{
	/* Create the semaphore used to access the UART Tx. */
	xTxMutex = xSemaphoreCreateMutex();
	configASSERT( xTxMutex );

	/*
	 * Create that task that handles the console itself.
	 * prvUARTCommandConsoleTask: The task that implements the command console.
	 * "CLI": Text name assigned to the task.  This is just to assist debugging.
	 * The kernel does not use this name itself.
	 * usStackSize: The size of the stack allocated to the task.
	 * NULL: The parameter is not used, so NULL is passed.
	 * uxPriority: The priority allocated to the task.
	 * NULL: A handle is not required, so just pass NULL.
	 */
	xTaskCreate(prvUARTCommandConsoleTask,
					"CLI",
					usStackSize,
					NULL,
					uxPriority,
					NULL );
}
/*-----------------------------------------------------------*/

static void prvUARTCommandConsoleTask( void *pvParameters )
{
char cRxedChar;
uint8_t ucInputIndex = 0;
uint8_t cmdcnt = 0;
uint8_t esc_cnt = 0;
uint8_t i = 0;
char *pcOutputString;
uint8_t ucOutputStringLen = 0;
static char cInputString[ cmdMAX_INPUT_SIZE ];
BaseType_t xReturned;
char com_str[cmdMAX_INPUT_SIZE + 1];

	( void ) pvParameters;

	/*
	 * Obtain the address of the output buffer.  Note there is no mutual
	 * exclusion on this buffer as it is assumed only one command console
	 * interface will be used at any one time.
	 */
	pcOutputString = FreeRTOS_CLIGetOutputBuffer();

	for( ;; )
	{
		/*
		 * Wait for the next character.  The while loop is used in case
		 * INCLUDE_vTaskSuspend is not set to 1 - in which case portMAX_DELAY
		 * will be a genuine block time rather than an infinite block time.
		 */
		while (pdTRUE != xSerialGetChar(NULL, (signed char *)&cRxedChar, portMAX_DELAY)) {}

		/* Ensure exclusive access to the UART Tx. */
		if( xSemaphoreTake( xTxMutex, cmdMAX_MUTEX_WAIT ) == pdPASS )
		{

			if (esc_cnt)
			{
				if (cRxedChar != cmdASCII_LBRACKET || esc_cnt == 2)
				{
					if ((cRxedChar == 'A') || (cRxedChar == 'B'))
					{
						printf("%s%s%s                                                                ",
							pcNewLine, CFG_BOARD_NAME,pcEndOfOutputMessage);

						/* clear current inputstring */
						ucInputIndex = 64; /*  todo : set max to 64bytes,*/
						while( ucInputIndex > 0 )
						{
							ucInputIndex--;
							cInputString[ ucInputIndex ] = '\0';
							printf("\b \b");
							//fflush(NULL);
						}
						ucInputIndex = 0;
						memset( cInputString, 0x00, cmdMAX_INPUT_SIZE );
						if(g_cmd_history_len > 0 ) {
							if (cRxedChar == 'A') /* up arrow */
							{
								/* Go to the past command in history */
								g_cmd_history_steps_from_head--;
								if (-g_cmd_history_steps_from_head >= g_cmd_history_len)
								{
									g_cmd_history_steps_from_head =
									    -(g_cmd_history_len - 1);
								}
							}
							else if (cRxedChar == 'B') /* down arrow */
							{
								/* Go to the recent command in history */
								g_cmd_history_steps_from_head++;

								if (g_cmd_history_steps_from_head > 1)
								{
									g_cmd_history_steps_from_head = 1;
								}
							}
							if (g_cmd_history_steps_from_head != 1)
							{
								int idx = g_cmd_history_head + g_cmd_history_steps_from_head;

								/* Circular buffer wrap around */
								if (idx < 0)
								{
									idx = idx + cmdMAX_CMD_CNT;
								}
								else if (idx >= cmdMAX_CMD_CNT)
								{
									idx = idx - cmdMAX_CMD_CNT;
								}

								for ((i = 0) && (ucInputIndex = 0); g_cmd_history[idx][i] != '\0'; i++)
								{
									cInputString[ucInputIndex] =
									    g_cmd_history[idx][i];
									ucInputIndex++;
								}
#if 0
								printf("%s%s%s%s", pcNewLine,
									CFG_BOARD_NAME,
									pcEndOfOutputMessage,
									cInputString);
#endif
								printf("%s", cInputString);
								//fflush(NULL);
							}
						}

					}

				esc_cnt = 0;
				cRxedChar = '\0';
				}

				else
				{
					esc_cnt = 2;
					cRxedChar = '\0';
				}
			}
			/* Was it the end of the line? */
			if( cRxedChar == '\n' || cRxedChar == '\r' )
			{
				if (ucInputIndex) {
					/* store current input string into the history cmd buf */
					g_cmd_history_head = (g_cmd_history_head + 1) % cmdMAX_CMD_CNT;
					for (i = 0; (i < ucInputIndex) && i < (cmdMAX_INPUT_SIZE - 1); i++)
					{
						g_cmd_history[g_cmd_history_head][i] =
						    cInputString[i];
					}
					g_cmd_history[g_cmd_history_head][i] = '\0';
					g_cmd_history_steps_from_head = 1;
					if (g_cmd_history_len < cmdMAX_CMD_CNT)
					{
						g_cmd_history_len++;
					}

					/* Just to space the output from the input. */
					printf("%s", pcNewLine);
					//fflush(NULL);

					/*
					 * Pass the received command to the command interpreter.
					 * The command interpreter is called repeatedly until it
					 * returns pdFALSE(indicating there is no more output) as
					 * it might generate more than one string.
					 */
					do
					{
						memset(pcOutputString, 0x00, strlen(pcOutputString));
						/* Get the next output string from the command interpreter. */
						xReturned = FreeRTOS_CLIProcessCommand( cInputString,
							pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE );

						if (ucOutputStringLen + strlen(pcOutputString) > 80)
						{
							printf("%s", pcNewLine);
							fflush(NULL);
							ucOutputStringLen = 0;
						}
						ucOutputStringLen += strlen(pcOutputString);
						/* Write the generated string to the UART. */
						printf("%s", pcOutputString);
						fflush(NULL);
					} while( xReturned != pdFALSE );

					memset(pcOutputString, 0x00, strlen(pcOutputString));
					ucOutputStringLen = 0;

					/*
					 * All the strings generated by the input command have been
					 * sent.  Clear the input string ready to receive the next command.
					 * Remember the command that was just processed first in case it is
					 * to be processed again.
					 */
					ucInputIndex = 0;
					memset( cInputString, 0x00, cmdMAX_INPUT_SIZE );
					printf("%s%s%s", pcNewLine, CFG_BOARD_NAME,
						pcEndOfOutputMessage);
					fflush(NULL);
				} else {
					/* just go to a new line when pressing ENTER on keyboard */
					printf("%s%s%s", pcNewLine, CFG_BOARD_NAME,
						pcEndOfOutputMessage);
					fflush(NULL);
				}
			}
			else if(cRxedChar == cmdASCII_ESC)
			{
				esc_cnt = 1;
			}
			else if( cRxedChar == '\r' )
			{
				/* Ignore the character. */
			}
			else if(( cRxedChar == cmdASCII_BS ))
			{
				/* Backspace was pressed. Erase the last input character - if any. */
				if( ucInputIndex > 0 )
				{
					ucInputIndex--;
					cInputString[ ucInputIndex ] = '\0';
					printf(" \b");
					fflush(NULL);
				}
			}else if(( cRxedChar == '\b' ) || ( cRxedChar == cmdASCII_DEL )) {
				if( ucInputIndex > 0 )
				{
					ucInputIndex--;
					cInputString[ ucInputIndex ] = '\0';
					printf("\b \b");
					fflush(NULL);
				}

			}else if ( cRxedChar == '\t' )
			{
				/* list all registed cmds if input string is null */
				if(!ucInputIndex || cInputString[0] == '\t')
				{
					printf("%s", pcNewLine);
					fflush(NULL);
					do
					{
						memset(pcOutputString, 0x00, strlen(pcOutputString));
						/* Get the next output string from the command interpreter. */
						xReturned = FreeRTOS_CLIProcessCommand( "help",
							pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE );

						if (ucOutputStringLen + strlen(pcOutputString) > 80)
						{
							printf("%s", pcNewLine);
							fflush(NULL);
							ucOutputStringLen = 0;
						}
						ucOutputStringLen += strlen(pcOutputString);
						/* Write the generated string to the UART. */
						printf("%s", pcOutputString);
						fflush(NULL);
					} while( xReturned != pdFALSE );
					printf("%s%s%s", pcNewLine, CFG_BOARD_NAME,
						pcEndOfOutputMessage);
					fflush(NULL);

					memset(pcOutputString, 0x00, strlen(pcOutputString));
					ucOutputStringLen = 0;
					cmdcnt = 0;
				}

				/*
				 * enter cli_tab_completion function
				 * cmdcnt is the cmd number found in register cmds.
				 * 0 means there is no match cmd found in register cmds,
				 * and will do nothing.
				 */
				else
					cmdcnt = cli_tab_completion(cInputString,
							pcOutputString,
							configCOMMAND_INT_MAX_OUTPUT_SIZE);

				if (cmdcnt == 1) {
					/*
					 * find only one register cmd,
					 * complete the cmd string.
					 */
					memcpy(cInputString, pcOutputString, strlen(pcOutputString));
					ucInputIndex = strlen(cInputString);

					/* send the input string to conlose again */
					printf("%s%s%s", pcNewLine, CFG_BOARD_NAME,
						pcEndOfOutputMessage);
					fflush(NULL);
					printf("%s", cInputString);
					fflush(NULL);
				} else if (cmdcnt > 1) {
					/*
					 * find more than one reister cmd,
					 * just list these options
					 */
					printf("%s", pcNewLine);
					fflush(NULL);
					printf("%s", pcOutputString);
					fflush(NULL);

					/* send the input string to conlose again */
					printf("%s", pcNewLine);
					fflush(NULL);
					printf("%s%s%s", pcNewLine, CFG_BOARD_NAME,
						pcEndOfOutputMessage);
					fflush(NULL);

					/* find the longest common str in the options */
					xReturned = find_common_str(cInputString,
							pcOutputString,
							com_str,
							configCOMMAND_INT_MAX_OUTPUT_SIZE);

					if (xReturned && (com_str[0] != '\0'))
					{
						for (i = 0; com_str[i] != '\0'; i++)
						{
							cInputString[ucInputIndex + i] =
							    com_str[i];
						}
						ucInputIndex += i;
					}
					printf("%s", cInputString);
					fflush(NULL);
				}
				memset(pcOutputString, 0x00, strlen(pcOutputString));
			}
			else if (cRxedChar == cmdASCII_ETX)
			{
				/* clear input str & output str */
				memset(pcOutputString, 0x00, strlen(pcOutputString));
				ucOutputStringLen = 0;

				ucInputIndex = 0;
				memset( cInputString, 0x00, cmdMAX_INPUT_SIZE );

				/* just begin a new line */
				printf("%s%s%s", pcNewLine, CFG_BOARD_NAME,
					pcEndOfOutputMessage);
				fflush(NULL);
			}
			else
			{
				/* Echo the character back. */
				//putchar(cRxedChar);
				//fflush(NULL);

				/*
				 * A non-control character was entered. Add it to the string
				 * entered so far.  When a \n is entered the complete string
				 * will be passed to the command interpreter.
				 */
				if( ( cRxedChar >= ' ' ) && ( cRxedChar <= '~' ) )
				{
					if( ucInputIndex < cmdMAX_INPUT_SIZE )
					{
						cInputString[ ucInputIndex ] = cRxedChar;
						ucInputIndex++;
					}
				}
			}

			/* Must ensure to give the mutex back. */
			xSemaphoreGive( xTxMutex );
		}
	}
}
/*-----------------------------------------------------------*/

void vOutputString( const char * const pcMessage )
{
	if( xSemaphoreTake( xTxMutex, cmdMAX_MUTEX_WAIT ) == pdPASS )
	{
		printf("%s", pcMessage);
		fflush(NULL);
		xSemaphoreGive( xTxMutex );
	}
}
/*-----------------------------------------------------------*/

void cli_init( void )
{
	xSerialPortInitMinimal(SERIAL_BAUDRATE, SERIAL_QUEUE_SIZE);
	vRegisterSampleCLICommands();
	vUARTCommandConsoleStart(FREERTOS_CLI_TASK_STACKSIZE, TASK_PRIORITY_CLI);
}
