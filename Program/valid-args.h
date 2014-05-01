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

void initValidArguments(int argc, char *argv[]) {
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
/*
				retcode = setLogLevel(_mylogLevel);
				handle_error(retcode, "LogLevel could not be set.\n",
						PROCESS_EXIT);
	*/
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
