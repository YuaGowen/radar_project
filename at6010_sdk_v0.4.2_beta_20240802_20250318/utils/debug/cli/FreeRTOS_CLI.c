/*
 * FreeRTOS+CLI V1.0.4
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

/* CLI includes. */
#include "FreeRTOS_CLI.h"

/*
 * If the application writer needs to place the buffer used by the CLI at a
 * fixed address then set configAPPLICATION_PROVIDES_cOutputBuffer to 1 in
 * FreeRTOSConfig.h, then declare an array with the following name and size in
 * one of the application files:
 *	char cOutputBuffer[ configCOMMAND_INT_MAX_OUTPUT_SIZE ];
*/
#ifndef configAPPLICATION_PROVIDES_cOutputBuffer
	#define configAPPLICATION_PROVIDES_cOutputBuffer 0
#endif

typedef struct xCOMMAND_INPUT_LIST
{
	const CLI_Command_Definition_t *pxCommandLineDefinition;
	struct xCOMMAND_INPUT_LIST *pxNext;
} CLI_Definition_List_Item_t;

/*
 * The callback function that is executed when "help" is entered.  This is the
 * only default command that is always present.
 */
static BaseType_t prvHelpCommand( char *pcWriteBuffer, size_t xWriteBufferLen,
				  size_t argc, char **argv);

/*
 * Return the number of parameters that follow the command name.
 */
static int8_t prvGetNumberOfParameters( const char *pcCommandString );

/*
 * The definition of the "help" command.  This command is always
 * at the front of the list of registered commands.
 */
static const CLI_Command_Definition_t xHelpCommand =
{
	"help",
	NULL,
	"\r\nhelp:\r\n Lists all the registered commands\r\n\r\n",
	prvHelpCommand,
	0
};

/*
 * The definition of the list of commands. Registered commands will be
 * added to this list.
 */
static CLI_Definition_List_Item_t xRegisteredCommands =
{
	&xHelpCommand,	/* The first command in the list is always the help cmd,
			 * defined in this file. */
	NULL		/* The next pointer is initialised to NULL,
			 * as there are no other registered commands yet. */
};

/*
 * A buffer into which command outputs can be written is declared here, rather
 * than in the command console implementation, to allow multiple command consoles
 * to share the same buffer.  For example, an application may allow access to the
 * command interpreter by UART and by Ethernet.  Sharing a buffer is done purely
 * to save RAM.  Note, however, that the command console itself is not re-entrant,
 * so only one command interpreter interface can be used at any one time.  For that
 * reason, no attempt at providing mutual exclusion to the cOutputBuffer array is
 * attempted.
 * configAPPLICATION_PROVIDES_cOutputBuffer is provided to allow the application
 * writer to provide their own cOutputBuffer declaration in cases where the
 * buffer needs to be placed at a fixed address (rather than by the linker).
 */
#if( configAPPLICATION_PROVIDES_cOutputBuffer == 0 )
	static char cOutputBuffer[ configCOMMAND_INT_MAX_OUTPUT_SIZE ];
#else
	extern char cOutputBuffer[ configCOMMAND_INT_MAX_OUTPUT_SIZE ];
#endif


/*-----------------------------------------------------------*/

BaseType_t FreeRTOS_CLIRegisterCommand( const CLI_Command_Definition_t * const pxCommandToRegister )
{
static CLI_Definition_List_Item_t *pxLastCommandInList = &xRegisteredCommands;
CLI_Definition_List_Item_t *pxNewListItem;
BaseType_t xReturn = pdFAIL;

	/* Check the parameter is not NULL. */
	configASSERT( pxCommandToRegister );

	/* Create a new list item that will reference the command being registered. */
	pxNewListItem = ( CLI_Definition_List_Item_t * ) pvPortMalloc( sizeof( CLI_Definition_List_Item_t ) );
	configASSERT( pxNewListItem );

	if( pxNewListItem != NULL )
	{
		taskENTER_CRITICAL();
		{
			/*
			 * Reference the command being registered from the newly created
			 * list item.
			 */
			pxNewListItem->pxCommandLineDefinition = pxCommandToRegister;

			/*
			 * The new list item will get added to the end of the list, so
			 * pxNext has nowhere to point.
			 */
			pxNewListItem->pxNext = NULL;

			/*
			 * Add the newly created list item to the end of the already existing
			 * list.
			 */
			pxLastCommandInList->pxNext = pxNewListItem;

			/* Set the end of list marker to the new list item. */
			pxLastCommandInList = pxNewListItem;
		}
		taskEXIT_CRITICAL();

		xReturn = pdPASS;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t FreeRTOS_CLIProcessCommand( const char * const pcCommandInput,
				       char * pcWriteBuffer,
				       size_t xWriteBufferLen  )
{
static const CLI_Definition_List_Item_t *pxCommand = NULL;
BaseType_t xReturn = pdTRUE;
const char *pcRegisteredCommandString;
size_t xCommandStringLength;

	/*
	 * Note:  This function is not re-entrant.  It must not be called from
	 * more than one task.
	 */

	if( pxCommand == NULL )
	{
		/* Search for the command string in the list of registered commands. */
		for( pxCommand = &xRegisteredCommands; pxCommand != NULL; pxCommand = pxCommand->pxNext )
		{
			pcRegisteredCommandString = pxCommand->pxCommandLineDefinition->pcCommand;
			xCommandStringLength = strlen( pcRegisteredCommandString );

			/*
			 * To ensure the string lengths match exactly, so as not to pick up
			 * a sub-string of a longer command, check the byte after the expected
			 * end of the string is either the end of the string or a space before
			 * a parameter.
			 */
			if( ( pcCommandInput[ xCommandStringLength ] == ' ' ) || ( pcCommandInput[ xCommandStringLength ] == 0x00 ) )
			{
				if( strncmp( pcCommandInput, pcRegisteredCommandString, xCommandStringLength ) == 0 )
				{
					/*
					 * The command has been found.  Check it has the expected
					 * number of parameters.  If cExpectedNumberOfParameters is -1,
					 * then there could be a variable number of parameters and no
					 * check is made.
					 */
					if( pxCommand->pxCommandLineDefinition->cExpectedNumberOfParameters >= 0 )
					{
						if( prvGetNumberOfParameters( pcCommandInput ) != pxCommand->pxCommandLineDefinition->cExpectedNumberOfParameters )
						{
							xReturn = pdFALSE;
						}
					}

					break;
				}
			}
		}
	}

	if( ( pxCommand != NULL ) && ( xReturn == pdFALSE ) )
	{
		/*
		 * The command was found, but the number of parameters with the command
		 * was incorrect.
		 * list the help string of that cmd.
		 */
		strncpy( pcWriteBuffer, pxCommand->pxCommandLineDefinition->pcHelpString, xWriteBufferLen );
		pxCommand = NULL;
	}
	else if( pxCommand != NULL )
	{
		int ret;
		size_t argc;
		char *argv[SUBCMD_NUM + 1];
		const struct cli_subcmd *pcRegisteredSubCommand;
		char pcCommandInputTmp[cmdMAX_INPUT_SIZE + 1];

		/* create argument list */
		pcCommandInputTmp[0] = 0x00;
		strcpy(pcCommandInputTmp, pcCommandInput);
		ret = cli_make_argv(&argc, &argv[0], (char *)pcCommandInputTmp,
					SUBCMD_NUM);
		if (ret != 0) {
			printf("(%s) not terminated: %c\n", __func__, ret);
			fflush(NULL);
			pxCommand = NULL;
			return pdFALSE;
		}

		if (!pxCommand->pxCommandLineDefinition->pcSubcommand)
		{
			/* Call the callback function that is registered to this command. */
			xReturn =
				pxCommand->pxCommandLineDefinition->pxCommandInterpreter(pcWriteBuffer,
											 xWriteBufferLen,
											 argc, &argv[0]);
		}
		else
		{
			pcRegisteredSubCommand = pxCommand->pxCommandLineDefinition->pcSubcommand;
			if (argc == 1)
			{
				/* list help string of that cmd */
				if(pxCommand->pxCommandLineDefinition->pcHelpString)
				{
					printf("%s\n", pxCommand->pxCommandLineDefinition->pcHelpString);
				}

				for(; pcRegisteredSubCommand->subcmd != NULL; pcRegisteredSubCommand++)
				{
					printf("  %-16s", pcRegisteredSubCommand->subcmd);
					if (pcRegisteredSubCommand->help &&
						strcmp(pcRegisteredSubCommand->help, pcRegisteredSubCommand->subcmd)) {
						printf("%s",
							pcRegisteredSubCommand->help);
					}
					printf("\n");
				}
				pxCommand = NULL;
				return pdFALSE;
			}

			/* find the handler of suncmd and execute it */
			for(; pcRegisteredSubCommand->subcmd != NULL; pcRegisteredSubCommand++)
			{
				if(!strcmp(argv[1], pcRegisteredSubCommand->subcmd))
				{
					ret = pcRegisteredSubCommand->handler(argc - 1, &argv[1]);
					break;
				}
			}
			if (ret != 0)
			{
				printf("invalid argument!\n");
				printf("  %-16s", pcRegisteredSubCommand->subcmd);
				if (pcRegisteredSubCommand->help) {
					printf("%s",
						pcRegisteredSubCommand->help);
				}
			}

#if 0
			if (ret == -EINVAL)
			{
				printf("invalid argument!\n");
				printf("  %-16s", pcRegisteredSubCommand->subcmd);
				if (pcRegisteredSubCommand->help) {
					printf("%s",
						pcRegisteredSubCommand->help);
				}
			}

			else if (ret == -ENOTSUP)
				printf("%s not supported!\n", argv[1]);
#endif

			if (argc > 1 && !pcRegisteredSubCommand->subcmd)
			{
				pcRegisteredSubCommand = pxCommand->pxCommandLineDefinition->pcSubcommand;
				/* list help string of that cmd */
				if(pxCommand->pxCommandLineDefinition->pcHelpString)
				{
					printf("%s\n", pxCommand->pxCommandLineDefinition->pcHelpString);
				}

				for(; pcRegisteredSubCommand->subcmd != NULL; pcRegisteredSubCommand++)
				{
					printf("  %-16s", pcRegisteredSubCommand->subcmd);
					if (pcRegisteredSubCommand->help) {
						printf("%s",
							pcRegisteredSubCommand->help);
					}
					printf("\n");
				}
			}

			xReturn = pdFALSE;
		}

		/*
		 * If xReturn is pdFALSE, then no further strings will be returned
		 * after this one, and	pxCommand can be reset to NULL ready to search
		 * for the next entered command.
		 */
		if( xReturn == pdFALSE )
		{
			pxCommand = NULL;
		}
	}
	else
	{
		/* pxCommand was NULL, the command was not found. */
		strncpy( pcWriteBuffer,
			"Command not recognised. Enter 'help' to view a list of available commands.\r\n",
			xWriteBufferLen );
		xReturn = pdFALSE;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

char *FreeRTOS_CLIGetOutputBuffer( void )
{
	return cOutputBuffer;
}
/*-----------------------------------------------------------*/

const char *FreeRTOS_CLIGetParameter( const char *pcCommandString,
				      UBaseType_t uxWantedParameter,
				      BaseType_t *pxParameterStringLength )
{
UBaseType_t uxParametersFound = 0;
const char *pcReturn = NULL;

	*pxParameterStringLength = 0;

	while( uxParametersFound < uxWantedParameter )
	{
		/*
		 * Index the character pointer past the current word.  If this is the start
		 * of the command string then the first word is the command itself.
		 */
		while( ( ( *pcCommandString ) != 0x00 ) && ( ( *pcCommandString ) != ' ' ) )
		{
			pcCommandString++;
		}

		/* Find the start of the next string. */
		while( ( ( *pcCommandString ) != 0x00 ) && ( ( *pcCommandString ) == ' ' ) )
		{
			pcCommandString++;
		}

		/* Was a string found? */
		if( *pcCommandString != 0x00 )
		{
			/* Is this the start of the required parameter? */
			uxParametersFound++;

			if( uxParametersFound == uxWantedParameter )
			{
				/* How long is the parameter? */
				pcReturn = pcCommandString;
				while( ( ( *pcCommandString ) != 0x00 ) && ( ( *pcCommandString ) != ' ' ) )
				{
					( *pxParameterStringLength )++;
					pcCommandString++;
				}

				if( *pxParameterStringLength == 0 )
				{
					pcReturn = NULL;
				}

				break;
			}
		}
		else
		{
			break;
		}
	}

	return pcReturn;
}
/*-----------------------------------------------------------*/

static BaseType_t prvHelpCommand( char *pcWriteBuffer,
				  size_t xWriteBufferLen,
				  size_t argc, char **argv)
{
static const CLI_Definition_List_Item_t * pxCommand = NULL;
BaseType_t xReturn;

	if( pxCommand == NULL )
	{
		/* Reset the pxCommand pointer back to the start of the list. */
		pxCommand = &xRegisteredCommands;
		/* ignore help itself */
		pxCommand = pxCommand->pxNext;
	}

	/*
	 * Return the next command help string, before moving the pointer on
	 * to the next command in the list.
	 */
	memset( pcWriteBuffer, 0x00, xWriteBufferLen );
	strncpy( pcWriteBuffer, pxCommand->pxCommandLineDefinition->pcCommand,
		 xWriteBufferLen);
	pxCommand = pxCommand->pxNext;

	if( pxCommand == NULL )
	{
		/*
		 * There are no more commands in the list, so there will be no more
		 * strings to return after this one and pdFALSE should be returned.
		 */
		strcat(pcWriteBuffer, "\r\n");
		xReturn = pdFALSE;
	}
	else
	{
		strcat(pcWriteBuffer, "  ");
		xReturn = pdTRUE;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

static int8_t prvGetNumberOfParameters( const char *pcCommandString )
{
int8_t cParameters = 0;
BaseType_t xLastCharacterWasSpace = pdFALSE;

	/* Count the number of space delimited words in pcCommandString. */
	while( *pcCommandString != 0x00 )
	{
		if( ( *pcCommandString ) == ' ' )
		{
			if( xLastCharacterWasSpace != pdTRUE )
			{
				cParameters++;
				xLastCharacterWasSpace = pdTRUE;
			}
		}
		else
		{
			xLastCharacterWasSpace = pdFALSE;
		}

		pcCommandString++;
	}

	/*
	 * If the command string ended with spaces, then there will have been
	 * too many parameters counted.
	 */
	if( xLastCharacterWasSpace == pdTRUE )
	{
		cParameters--;
	}

	/*
	 * The value returned is one less than the number of space delimited words,
	 * as the first word should be the command itself.
	 */
	return cParameters;
}

uint8_t find_common_str(const char * const pcCommandInput,
			char * pcWriteBuffer,
			char * com_str,
			size_t xWriteBufferLen)
{
uint8_t cur_com = 0;
uint8_t com = cmdMAX_INPUT_SIZE;
uint8_t len = 0;
char pcWriteBufferTmp[xWriteBufferLen];
char pcCommandInputTmp[cmdMAX_INPUT_SIZE + 1];
char *argv[SUBCMD_NUM + 1];
size_t argc;
int ret;
uint8_t i = 0;

	pcWriteBufferTmp[0] = 0x00;
	strcpy(pcWriteBufferTmp, pcWriteBuffer);
	ret = cli_make_argv(&argc, &argv[0], (char *)pcWriteBufferTmp,
				128);
	if (ret != 0) {
		printf("(%s) not terminated: %c\n", __func__, ret);
		fflush(NULL);
		return pdFALSE;
	}

	for (i = 0; i < argc -1 && argc > 1; i++)
	{
		len = cmdMAX_INPUT_SIZE;
		while((*argv[i] == *argv[i+1]) && (*argv[i] != '\0') && (*argv[i+1] != '\0')&& (len > 0))
		{
			cur_com++;
			len--;
			argv[i]++;
			argv[i+1]++;
		}
		com = (cur_com < com) ? cur_com : com;
		cur_com = 0;
	}

	pcCommandInputTmp[0] = 0x00;
	strcpy(pcCommandInputTmp, pcCommandInput);
	ret = cli_make_argv(&argc, &argv[0], (char *)pcCommandInputTmp,
				SUBCMD_NUM);
	if (ret != 0) {
		printf("(%s) not terminated: %c\n", __func__, ret);
		fflush(NULL);
		return pdFALSE;
	}


	com_str[0] = 0x00;
	if (argc == 1)
	{
		com = com ? com - strlen(argv[0]) : com;
		for (i = 0; i < com; i++)
			com_str[i] = pcWriteBufferTmp[strlen(argv[0]) + i];
	}
	else if (argc > 1)
	{
		com = com ? com - strlen(argv[1]) : com;
		for (i = 0; i < com; i++)
			com_str[i] = pcWriteBufferTmp[strlen(argv[1]) + i];
	}
	com_str[i] = '\0';

	return pdTRUE;
}

uint8_t cli_tab_completion(const char * const pcCommandInput,
			   char * pcWriteBuffer,
			   size_t xWriteBufferLen)
{
CLI_Definition_List_Item_t *pxCommand = &xRegisteredCommands;
const char *pcRegisteredCommandString;
const struct cli_subcmd *pcRegisteredSubCommand;
size_t pcCommandInputLength;
size_t pcSubCommandInputLength;
uint8_t cmdcnt = 0;
uint8_t ucWriteBufferLen = 0;
char pcWriteBufferTmp[xWriteBufferLen];
char pcCommandInputTmp[cmdMAX_INPUT_SIZE + 1];

/* add argv to enable second tab*/
char *argv[SUBCMD_NUM + 1]; /* +1 reserved for NULL */
size_t argc;
/* return pdFALSE to break cli cmd process, pdTRUE to continue */
int ret;

	/* create argument list */
	pcCommandInputTmp[0] = 0x00;
	strcpy(pcCommandInputTmp, pcCommandInput);
	ret = cli_make_argv(&argc, &argv[0], (char *)pcCommandInputTmp,
				SUBCMD_NUM);
	if (ret != 0) {
		printf("(%s) not terminated: %c\n", __func__, ret);
		fflush(NULL);
		return pdFALSE;
	}

	/*
	 * Note:  This function is not re-entrant.
	 * It must not be called from more thank one task.
	 */
	for(; pxCommand != NULL; pxCommand = pxCommand->pxNext )
	{
		pcRegisteredCommandString =
			pxCommand->pxCommandLineDefinition->pcCommand;
		pcCommandInputLength = strlen( argv[0] );

		if( strncmp( argv[0], pcRegisteredCommandString, pcCommandInputLength ) == 0 )
		{
			if (argc == 1) {
				if(pxCommand->pxCommandLineDefinition->pcSubcommand &&
					!strcmp(argv[0], pcRegisteredCommandString))
				{
					pcWriteBufferTmp[0] = 0x00;

					pcRegisteredSubCommand = pxCommand->pxCommandLineDefinition->pcSubcommand;
					strcat(pcWriteBufferTmp, pcRegisteredCommandString);
					strcat(pcWriteBufferTmp, " ");

					for(; pcRegisteredSubCommand->subcmd != NULL; pcRegisteredSubCommand++)
					{
						if (ucWriteBufferLen + strlen(pcRegisteredSubCommand->subcmd) > 80)
						{
							strcat(pcWriteBufferTmp, "  \n");
							ucWriteBufferLen = 0;
						}
						if (cmdcnt > 0 && ucWriteBufferLen != 0)
							strcat(pcWriteBufferTmp, "  ");
						strcat(pcWriteBufferTmp,
							pcRegisteredSubCommand->subcmd);
						ucWriteBufferLen = ucWriteBufferLen +
							strlen(pcRegisteredSubCommand->subcmd)+
							strlen("  ");
						cmdcnt++;
					}

					if (cmdcnt > 1)
					{
						memcpy(pcWriteBuffer, pcWriteBufferTmp +
							strlen(pcRegisteredCommandString) +
							strlen(" "), strlen(pcWriteBufferTmp) -
							strlen(pcRegisteredCommandString) - strlen(" "));
					}
					else if (cmdcnt == 1)
						memcpy(pcWriteBuffer, pcWriteBufferTmp, strlen(pcWriteBufferTmp));
				}
				else
				{
					/*
					 * Search for the command string in the list of
					 * registered commands.
					 */
					if (ucWriteBufferLen + strlen(pcRegisteredCommandString) > 80)
					{
						strcat(pcWriteBuffer, "  \n");
						ucWriteBufferLen = 0;
					}
					if (cmdcnt > 0 && ucWriteBufferLen != 0)
						strcat(pcWriteBuffer, "  ");

					/*
					 * The command has been found.
					 * Copy it to pcWriteBuffer.
					 */
					strncat(pcWriteBuffer, pcRegisteredCommandString,
						strlen(pcRegisteredCommandString));
					ucWriteBufferLen = ucWriteBufferLen
						+ strlen(pcRegisteredCommandString)
						+ strlen("  ");
					cmdcnt++;
				}
			}
			else if (argc == 2)
			{
				pcWriteBufferTmp[0] = 0x00;

				if (!pxCommand->pxCommandLineDefinition->pcSubcommand)
				{
					/* cmd register NULL subcmd in cli, return 0. */
					memset(pcWriteBuffer, 0x00, xWriteBufferLen);
					cmdcnt = 0;
				}
				else
				{
					pcRegisteredSubCommand = pxCommand->pxCommandLineDefinition->pcSubcommand;
					strcat(pcWriteBufferTmp, pcRegisteredCommandString);
					strcat(pcWriteBufferTmp, " ");

					for(; pcRegisteredSubCommand->subcmd != NULL; pcRegisteredSubCommand++)
					{
						pcSubCommandInputLength = strlen( argv[1] );
						if( strncmp(argv[1], pcRegisteredSubCommand->subcmd, pcSubCommandInputLength) == 0 )
						{
							if (ucWriteBufferLen + strlen(pcRegisteredSubCommand->subcmd) > 80)
							{
								strcat(pcWriteBufferTmp, "  \n");
								ucWriteBufferLen = 0;
							}
							if (cmdcnt > 0 && ucWriteBufferLen != 0)
								strcat(pcWriteBufferTmp, "  ");
							strcat(pcWriteBufferTmp,
								pcRegisteredSubCommand->subcmd);
							ucWriteBufferLen = ucWriteBufferLen +
								strlen(pcRegisteredSubCommand->subcmd)+
								strlen("  ");
							cmdcnt++;
						}
					}

					if (cmdcnt > 1)
					{
						memcpy(pcWriteBuffer, pcWriteBufferTmp +
							strlen(pcRegisteredCommandString) +
							strlen(" "), strlen(pcWriteBufferTmp) -
							strlen(pcRegisteredCommandString) - strlen(" "));
					}
					else if (cmdcnt == 1)
						memcpy(pcWriteBuffer, pcWriteBufferTmp, strlen(pcWriteBufferTmp));
				}
				break;
			}
		}
	}

	return cmdcnt;
}
