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

#include "Log-Level.h"
#define TOT_SHM_SIZE 65536
#define MIM_SHM_BLOCK_SIZE 4
#define MAX_FILE_LENGTH 32000
#include <arpa/inet.h>  /* for sockaddr_in, inet_addr() and inet_ntoa() */
#include <errno.h>
#include<math.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
/* for memset() and ... */
#include <sys/socket.h> /* for socket(), bind(), recv, send(), and connect() */
#include <sys/types.h>
#include <unistd.h>     /* for close() */

#include <itskylib.h>
#include "my.h" // global structs
#include "shm_control.c" // global structs
#include "myfunctions.c"
/* ====================================================================================== */
/*  all needed for Shared Memory */
#include "shm.c"

int shm_id;
/* ====================================================================================== */

/* all needed for handle Files */
#include "handleFiles.c"
#define BUFSIZE 128   /* Size of receive buffer */
#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define MAXRECWORDS 30000/* Maximum of words receiving from the client */
#define SERVERPORT_ARG "-p";
#define LOGLEVEL_ARG "-l";
#define SERVERNAME "Server";
int LOGLEVEL = 4;
int _MAX_LENGTH_ARG = 5; // defines the maximum Length of arguments. e.g. "-l"

/* all global variables for Arguments */
char _logLevel_arg[10] = LOGLEVEL_ARG
;
char _serverPort_arg[10] = SERVERPORT_ARG
;

/* global vars for TCP-Server */
int servSock; /* Socket descriptor for server */
int clntSock; /* Socket descriptor for client */
struct sockaddr_in squareServAddr; /* Local address */
struct sockaddr_in squareClntAddr; /* Client address */
unsigned short ServerPort; /* Server port */
unsigned int clntAddrLen; /* Length of client address data structure */

struct shm_ctr_struct *shm_ctr;
char *default_Filename = "NULL";

struct validArgs {
	int isSet;
	char arg[10];
};

struct validArgs validArguments[5];

#include "valid-args.h"

int setTCPServer();

int setup_shm() {
	/* set up shared Memory */
	printf("Setting up shared Memory ...");

	/* create REF File, if it not exists */
	create_if_missing(REF_FILE, S_IRUSR | S_IWUSR);

	/*create shm 'unique' key */
	key_t shm_key = ftok(REF_FILE, 1);
	if (shm_key < 0) {
		handle_error(-1, "ftok failed", PROCESS_EXIT);
	}
	create_shm(shm_key, "create", "shmget failed", IPC_CREAT | IPC_EXCL);
	shm_id = create_shm(shm_key, "create", "shmget failed", 0);

	return 1;
}

void my_handler(int signo) {
	if (signo == SIGUSR1) {
		printf(
				"Received other than SIGINT. Server is cleaning up shared memory and is going to close...\n");
		/* clean up shared memory */
		cleanup(shm_id);
	} else {
		printf(
				"Received SIGINT. Server is cleaning up shared memory and is going to close...\n");
		/* clean up shared memory */
		cleanup(shm_id);
		exit(1);
	}
}

/* initializing Shared Memory Control Set*/
int initshm(char *shm_start) {
	printf("Creating shm Control Set ...");
	shm_ctr = malloc(sizeof(struct shm_ctr_struct));
	shm_ctr->shm_size = TOT_SHM_SIZE;
	shm_ctr->isfree = TRUE;
	shm_ctr->isLast = TRUE;
	shm_ctr->next = shm_ctr;
	shm_ctr->prev = shm_ctr;
	shm_ctr->filename = "NULL";
	shm_ctr->filedata = shm_start;
	printf("... Done\n");
	printf("Shared Memory ID = %i\n", shm_id);
	printf("Filename is: %c\n", shm_ctr->filename);
	printf("Shared Memory Start location = %p\n", &(shm_ctr->filedata));
	return TRUE;
}

int main(int argc, char *argv[]) {
	int retcode;
	printf("\nServer started. Is now initializing the setup...\n");

	signal(SIGINT, my_handler);

	retcode = setup_shm();
	handle_error(retcode, "Shared Memory could not be created.\n",
			PROCESS_EXIT);

	char *shm_start = shmat(shm_id, NULL, 0);
	printf("... Done\n");

	/* init shared memory control*/
	retcode = initshm(shm_start);
	handle_error(retcode, "Could not create Shared Memory Control Set...\n",
			PROCESS_EXIT);

	/* Testfile for testing memory control */
	printf("\n");
	int filesize = 1056;
	printf("Want to write filename with size=%i to shm\n", filesize);
	char * testfilename = "Test.txt";
	printf("Adress of shm Place to check is %x\n", shm_ctr);
	struct shm_ctr_struct *place = find_shm_place(shm_ctr, filesize);
	printf("Checked a good address is:  %x\n", place);

	/*if there is no good place found, devide shm blocks */
	if (place == FALSE) {
		printf(
				"0 is not valid. So there is no good place to write the file into... Trying no to devide the Shared Memory...\n");
		int block_size_needed = round_up_int(filesize);
		retcode = devide(shm_ctr, block_size_needed);
		handle_error(retcode,
				"Could not devide the shared memory for the needed size...\n",
				PROCESS_EXIT);
		place = find_shm_place(shm_ctr, filesize);
	}

// if address of founded place + size of place < than
	//	if ((&(place->filedata) + place->shm_size)< (&(shm_ctr->filedata) + TOT_SHM_SIZE)) {
	printf("Address of place = %x\n", place);
	if (!place == 0) {
		printf("Inside of checking if return of place is valid...\n");
		place->isfree = FALSE;
		place->filename = testfilename;

	}
//#ifdef DEBUG
	//if (LOGLEVEL >= LOG_INFORMATIONAL) {
	//LOG_TRACE(LOG_INFORMATIONAL, "Will now output all existing shared memory blocks\n");
	print_all_shm_blocks(shm_ctr);
	//}
//#endif

	/* ====================================================================================== */

	printf("Setting up the valid arguments...");

	setValidServerArguments(); //setting up all valid arguments
	printf("... Done\n");

	/* if no arguments is chosen, output the usage of the Server */
	if (argc == 1) {
		usage();
	} else {
		printf("Verify valid arguments ...\n");
		initValidServerArguments(argc, argv);
	}

	if (validArguments[1].isSet == 0) {
		printf(
				"There was no argument for the Server-Port. It will no be set to default = 7000\n");
		ServerPort = 7000;
		validArguments[1].isSet = 1;

	} else {

	}

	retcode = setTCPServer();
	handle_error(retcode, "TCP Server settings could not be established!\n",
			PROCESS_EXIT);
	ServerListen();

	/* clean up shared memory */
	cleanup(shm_id);

}

int setTCPServer() {
	int retcode;
	printf("Set up TCP-Server settings ...\n");
	//printf("LOGLEVEL_DEBUG = %i",LOGLEVEL_DEBUG);

	/* Create socket for incoming connections */
	servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	handle_error(servSock, "socket() failed", PROCESS_EXIT);

	/* Construct local address structure */
	memset(&squareServAddr, 0, sizeof(squareServAddr));
	/* Zero out structure */
	squareServAddr.sin_family = AF_INET; /* Internet address family */
	squareServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
	squareServAddr.sin_port = htons(ServerPort); /* Local port */

	/* Bind to the local address */
	retcode = bind(servSock, (struct sockaddr *) &squareServAddr,
			sizeof(squareServAddr));
	handle_error(retcode, "bind() failed", PROCESS_EXIT);

	/* Mark the socket so it will listen for incoming connections */
	retcode = listen(servSock, MAXPENDING);
	handle_error(retcode, "listen() failed", PROCESS_EXIT);

	return 1;
}

int setLogLevel(int logLevel) {

#ifndef DEBUG
#define  DEBUG
#endif
	LOGLEVEL = logLevel;
	return 1;

}

void ServerListen() {
	printf("Server is now going to Listening Mode for Clients.\n");
	printf("Client can connect to Server on Port %i\n", ServerPort);
	while (TRUE) { /* Run forever */
		/* Set the size of the in-out parameter */
		clntAddrLen = sizeof(squareClntAddr);

		/* Wait for a client to connect */
		printf("Waiting for Client for connecting...\n");
		//printf("Client Socket = %n",clntSock);
		clntSock = accept(servSock, (struct sockaddr *) &squareClntAddr,
				&clntAddrLen);
		handle_error(clntSock, "accept() failed", PROCESS_EXIT);

		/* clntSock is connected to a client! */
		printf("Warning = %i\t Loglevel = %i\n", LOG_WARNING, LOGLEVEL);
#ifdef DEBUG
		if (LOGLEVEL >= LOG_WARNING) {
			LOG_TRACE(LOG_WARNING, "Test Log Warning\n");
			printf("Handling Client %s\n", inet_ntoa(squareClntAddr.sin_addr));
		}
#endif

		handle_tcp_client(clntSock);
	}
	/* NOT REACHED: */
	exit(0);
}

void runClientCommand(char *recMessage[], char *command) {

	if (strcmp(command, "CREATE") == 0) {
		printf("Will no try to create a new file...\n");
		//int strlen(filecontent);
		char *filecontent = malloc(sizeof(char) * MAX_FILE_LENGTH);
		filecontent = getFileContent(recMessage);
		//filesize = strlen(filecontent);
	  	char *filename = recMessage[2];
		printf("Filesize = %i \t Content = %s\n", strlen(filecontent),
				filecontent);
		char *returnvalue;
		writeNewFile(shm_ctr, filename, filecontent, strlen(filecontent));
	}

}

void breakCharArrayInWords(char *recMessage[], char *recBuffer[]) {
	printf("## Now in breakCharArrayinWords ##\n");

	/* break now the received Message into a string array where the sign " " breaks words */
	char breaksign[] = " ";
	char *token = malloc(sizeof(char) * MAX_FILE_LENGTH);
	int count = 0;

	/* get the first token */
	token = strtok(recBuffer, breaksign);
	printf("%i: Token = %s and Size of token = %i\n", count, token,
			strlen(token));
	//printf("recMessage[count]= %s\n", recMessage[count]);
	//memset(recMessage[count], '\0', sizeof( recMessage[count] ));
	//clear String
	//char *tmp = token;
	//printf("%i: string = %s and Size of string = %i\n", count, tmp, strlen(tmp));
	//recMessage[count] = malloc(sizeof(char) * 1024);
	recMessage[count] = token;
	printf("recMessage[%i]= %s\n", count, recMessage[count]);
	count++;

	/* walk through other tokens */
	while (token != NULL) {
		token = strtok(NULL, breaksign);
	//	recMessage[count] = malloc(sizeof(char) * 1024);
		recMessage[count] = token;
		printf("recMessage[count]= %s\n", recMessage[count]);
		count++;
	}
}

void sendMessage() {

}

void handle_tcp_client(int clntSocket) {
	int retrcode;
	//char *recMessage[MAXRECWORDS]; /* array to save the single words of the received message */
	char recBuffer[MAXRECWORDS]; /* Buffer for  string */
	int recvMsgSize; /* Size of received message */

	//reset recBuffer

	while (TRUE) {
		//	char recBuffer[BUFSIZE];
		memset(recBuffer, '\0', sizeof( recBuffer ));
		printf("\nbefore Handling Client...\n\n");
		print_all_shm_blocks(shm_ctr);
		char *recMessage[MAXRECWORDS]; /* array to save the single words of the received message */

		/* reset Buffer for next transmission */
		//memset(recBuffer, '\0', sizeof( recBuffer ));

		/* Receive message from client */
		printf("Waiting for reveicing message from Client.\n");
		recvMsgSize = recv(clntSocket, recBuffer, BUFSIZE - 1, 0);
		handle_error(recvMsgSize, "recv() failed", PROCESS_EXIT);
		printf("Received message from Client %s: %s\n",
				inet_ntoa(squareClntAddr.sin_addr), recBuffer);
		printf("Received Message Size = %i\n", recvMsgSize);
		printf("rec Buffer = %s\n", recBuffer);

		printf("\nbefore break Array into Words...\n\n");
	 	print_all_shm_blocks(shm_ctr);
		breakCharArrayInWords(recMessage, recBuffer);

#ifdef DEBUG
		if (LOGLEVEL >= LOG_NOTICE) {
			LOG_TRACE(LOG_NOTICE,
					"Checking now if expected length of message is the effective length. If yes, transmission was ok.\n");
			//char *tmp;
			LOG_TRACE(LOG_NOTICE, "Size should be: %s\t Size is: %i\n",
					recMessage[0], recvMsgSize);
			/*printf("Size should be: %s\t Size is: %i\n", recMessage[0],
			 recvMsgSize);
			 */}
#endif
		/* check is effective message is equal to expected message size */
		int effLength = (int) atoi(recMessage[0]);
		if (effLength == recvMsgSize) {
#ifdef DEBUG
			if (LOGLEVEL >= LOG_NOTICE) {
				printf("ALL OK!\n\n");
			}
#endif

			retrcode = getValidServerCommand(recMessage[1]);
			/* check if 1st word of message is a valid command */
#ifdef DEBUG
			if (LOGLEVEL >= LOG_NOTICE) {
				printf("Command is \"%s\" and has a length of %i\n",
						recMessage[1], strlen(recMessage[1]));
				if (retrcode) {
					printf("It is a valid command: %s\n", recMessage[1]);
				};
			}
#endif

			/* if command is valid */
			if (retrcode) {
				runClientCommand(recMessage, recMessage[1]);

			}

		}

		if (recvMsgSize < 3) {

			char msg[BUFSIZE] =
					"Wrong number of arguments. Please write [CREATE|DELETE] <filename>";
			strncpy(recBuffer, msg, sizeof(msg));
			recBuffer[sizeof(msg) - 1] = '\0';

			ssize_t sentSize = send(clntSocket, recBuffer, strlen(recBuffer),
					0);

			break;
		}

		recBuffer[recvMsgSize] = '\000'; // set End Termination at the end of the Buffer

		//int x = 5;
		//	int y = x * x;
		//sprintf(recBuffer, "%12d", y);
		int sendMsgSize = strlen(recBuffer);
		ssize_t sentSize = send(clntSocket, recBuffer, sendMsgSize, 0);
		/*if (sentSize != recvMsgSize) {
		 die_with_error("send() failed");
		 }*/
		/* See if there is more data to receive in the next round...*/
		//free(recBuffer);
	}

	close(clntSocket); /* Close client socket */
}
