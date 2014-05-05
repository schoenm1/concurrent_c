enum LOGLEVELS {
	LOG_EMERGENCY,
	LOG_ALERT,
	LOG_CRITICAL,
	LOG_ERROR,
	LOG_WARNING,
	LOG_NOTICE,
	LOG_INFORMATIONAL,
	LOG_DEBUG
} LOG_LEVEL;
/*
 enum SERVER_COMMANDS {
 CREATE, UPDATE, DELETE, LIST, READ
 } SERVER_COMMANDS;
 */

char * getFileContent(char *recMessage[]) {
//	printf("Now in funktion getFileContent()\n");
	int i;
	char * retchar = malloc(sizeof(char) * MAX_FILE_LENGTH);
	memset(retchar, '\0', sizeof(retchar)); //clear String
	strcat(retchar, recMessage[3]);

//printf("Momentaner String = %s\n",retchar);

	for (i = 4; i < sizeof(recMessage) - 1; i++) {
//	printf("Setze nur fŸr i=%i Strings zusammen\n",i);
		strcat(retchar, " ");
		strcat(retchar, recMessage[i]);
	}
//printf("Message String = %s\n",retchar);
	return retchar;

}

int getValidServerCommand(char *command) {
//printf("command=\"%s\" size = %i\n",command,strlen(command));
//printf("command should  be=\"%s\" length = %i\n","CREATE",strlen("CREATE"));
	printf("Command = %s\n", command);
	if (strcmp(command, "CREATE") == 0)
		return TRUE;
	if (strcmp(command, "DELETE") == 0)
		return TRUE;
	if (strcmp(command, "READ") == 0)
		return TRUE;
	if (strcmp(command, "UPDATE") == 0)
		return TRUE;
	if (strcmp(command, "LIST") == 0)
		return TRUE;

	printf("no match found...\n");
	return FALSE;
}

char * getLogLevel(int log_level, char *lvl) {
//	char *lvl = malloc(sizeof(char) * 24);

	switch (log_level) {

	case LOG_EMERGENCY:
		lvl = "LOG_EMERGENCY";
		break;
	case LOG_ALERT:
		lvl = "LOG_ALERT";
		break;
	case LOG_CRITICAL:
		lvl = "LOG_CRITICAL";
		break;
	case LOG_ERROR:
		lvl = "LOG_ERROR";
		break;
	case LOG_WARNING:
		lvl = "LOG_WARNING";
		break;
	case LOG_NOTICE:
		lvl = "LOG_NOTICE";
		break;
	case LOG_INFORMATIONAL:
		lvl = "LOG_INFORMATIONAL";
		break;
	case LOG_DEBUG:
		lvl = "LOG_DEBUG";
		break;
	}
	//printf("**************** - LOGLEVEL = %s - ********************************\n",lvl);
	return lvl;
}

/* LOG_TRACE(log level, format, args ) */
void LOG_TRACE(int lvl, char *msg, ...) {
//LOGLEVEL MY_LOGLEVEL;
	char buf[1024];
	char *buff = (char *) malloc(sizeof(char) * 1024);
	char *L_LEVEL = malloc(sizeof(char) * 24);

	L_LEVEL = getLogLevel(lvl, L_LEVEL);
	printf("# %s %s:\t", __DATE__, __TIME__);

	va_list va;
	va_start(va, msg);
	vsprintf(buf, msg, va);
	sprintf(buff, "%s\t%s\n", L_LEVEL, buf);
	printf("%s", buff);
	va_end(va);
	free(buff);
	//free(L_LEVEL);
}

void setValidServerArguments() {
	/* init LOGLEVEL */
	validArguments[0].isSet = 0;
	char _logLevel[5] = LOGLEVEL_ARG;
	strncpy(validArguments[0].arg, _logLevel, strlen(_logLevel));

	/* init Server-Port */
	validArguments[1].isSet = 0;
	char _serverPort[5] = SERVERPORT_ARG;
	strncpy(validArguments[1].arg, _serverPort, strlen(_serverPort));
}

int setServerPort(int myserverPort) {
	int _myServerPort = myserverPort;
	if (_myServerPort <= 1024 || _myServerPort > 65535) {
		printf(
				" ... Wrong Argument! Server-Port can be 1024 - 65535.\nServer Port will now be set to default = 7000 ... ");
		_myServerPort = 7000;

		ServerPort = _myServerPort;
		validArguments[1].isSet = 1;
		return 1;
	} else {

		ServerPort = _myServerPort;
		validArguments[1].isSet = 1;
		return 1;
	}
	return -1;
}

void initValidServerArguments(int argc, char *argv[]) {
	int i;
	int retcode;
	int counter_validArgs = 0;

	/* Parse all Arguments (except arg[0]. This is File itself) */
	for (i = 1; i < argc; i++) {
		printf(
				"\n----------------------------------------------------------\nArgument No. %i\t",
				i);
		printf("Value = %s\t", argv[i]);

		/* if "-l" is Arg, then set Log-Level */
		if (strcmp(argv[i], _logLevel_arg) == 0) {
			printf("-> Hit for Loglevel\n\n");
			int _mylogLevel = (int) atoi(argv[i + 1]);
			//printf("********* arg = %i",validArguments[0].isSet);
			if (validArguments[0].isSet == 0) {
				retcode = setLogLevel(_mylogLevel);
				handle_error(retcode, "LogLevel could not be set.\n",
						PROCESS_EXIT);
				validArguments[0].isSet = 1;
				printf("... Done\n");
			} else {
				printf(
						"LogLevel was already set. New argument will be ignored.\n");
			}
			i++;
		}

		/* parse for Server-Port */
		if (strcmp(argv[i], _serverPort_arg) == 0) {
			printf("-> Hit for Server-Port\n\n");
			int _myServerPort = (int) atoi(argv[i + 1]);
			if (validArguments[1].isSet == 0) {
				printf("Set up now Server Port to %i ... ", _myServerPort);
				retcode = setServerPort(_myServerPort);
				handle_error(retcode, "Server-Port could not be set.\n",
						PROCESS_EXIT);
				printf("   ... Done\n");
			} else {
				printf(
						"ServerPort was already set. New argument will be ignored.\n");
			}
			i++;
		}

	}
}

void usage() {
	printf("\nFollowing parameters are allowed:\n");
	printf("Parameter \tDescription \t\tDefault Value\tExample\n");
	printf(
			"==========================================================================\n");
	printf("-l \t\tLog-Level \t\tnot set yet \t./Server -l [0-7]\n");
	printf("-p \t\tServer Port \t\t7000\t\t./Server -p [1024-65535]\n");
	printf(
			"==========================================================================\n");
	printf(
			"\nYou can also combine all the parameters. If you would use a parameter twice ore more,\n the following parameters will be ignored.\n\n\n");
}
