/*
 * File: 		Main.c
 * Author: 		Micha Schšnenberger
 * Modul:		Concurrent Programming in C
 *
 * Created:     07.04.2014
 * Project:		https://github.com/schoenm1/concurrent_c.git
 */

/* ------------------ How to user this Program ------------------
 1) ...
 2) ...
 3) ...


 ---------------------------------------------------------------*/

#include <arpa/inet.h>  /* for sockaddr_in, inet_addr() and inet_ntoa() */
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
#include <string.h>     /* for memset() and ... */
#include <sys/socket.h> /* for socket(), bind(), recv, send(), and connect() */
#include <sys/types.h>
#include <unistd.h>     /* for close() */

#include <itskylib.h>
#include "Log-Level.h"

#define RCVBUFSIZE 128   /* Size of receive buffer */
#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define SERVERPORT_ARG "-p";

unsigned short squareServPort;     /* Server port */




struct validArgs {
	int isSet;
	char arg[];

};

struct validArgs validArguments[5];

void initValidArguments(int argc, char *argv[]) {
	int i;
	int retcode;
	char _logLevel[10] = LOGLEVEL_ARG;
	char _serverPort[10] =SERVERPORT_ARG;
	int counter_validArgs = 0;

	/* Parse all Arguments (except arg[0]. This is File itself) */
	for (i = 1; i < argc; i++) {
		printf("Argument No. %i\t", i);
		printf("Value = %s\n", argv[i]);

		/* if "-l" is Arg, then set Log-Level */
		if (strcmp(argv[i], _logLevel) == 0) {
			printf("Treffer fuer Loglevel\n\n");
			int _mylogLevel = (int) atoi(argv[i + 1]);
			printf("Loglevel = %i\n", _mylogLevel);
			retcode = setLogLevel(_mylogLevel);
			handle_error(retcode, "LogLevel could not be set\n", PROCESS_EXIT);
			i++;
		}


		/* parse for Server-Port */
		if (strcmp(argv[i], _logLevel) == 0) {
//hier fehlt noch die Zuweisung des Portes

			i++;
		}




	}
}
void usage(const char *argv0, const char *msg) {
	if (msg != NULL && strlen(msg) > 0) {
		printf("%s\n\n", msg);
	}
	printf("Usage MUST\n==============\n");
	printf("%s -p <Server Port>\n", argv0);
	printf("Usage OPTIONAL\n====================\n");
	printf("-l <int Loglevel> {0=EMERGENCY, 1=ALERT, INFORMATIONAL}\n");
	exit(1);
}

void validatingArgs(int argc, char *argv[]) {
	int i;

	for (i = 0; i < argc; i++) {

		//if (strcmp(thread_data_array[count2].filename, valid_file[count])


	}

}

int main(int argc, char *argv[]) {

	initValidArguments(argc, argv);
	//initValidArgs();

	/*char valid_inputs[100][255];

	 printf("\nValidating now the arguments and set the configuration of the Server...\n");
	 printf("Loglevel = %i",(int) argv[2]);



	 */

	//setLogLevel(myint);


	int retcode;
	if (is_help_requested(argc, argv)) {
		usage(argv[0], "");
	}

}
