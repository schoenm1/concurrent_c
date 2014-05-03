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
//#include "handleFiles.c"
#define BUFSIZE 128   /* Size of receive buffer */
#define MAXPENDING 5    /* Maximum outstanding connection requests */
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
//char value[10];
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
	printf("Filename is: %c\n", shm_ctr->filedata);
	printf("Shared Memory Start location = %p\n", &(shm_ctr->filedata));
	return TRUE;
}

int main(int argc, char *argv[]) {
	int retcode;
	printf("\nServer started. Is now initializing the setup...\n");

	signal(SIGINT, my_handler);

	/* set up default Log-Level */
	//retcode = setLogLevel(4);
	//handle_error(retcode, "LogLevel could not be set.\n", PROCESS_EXIT);
	//printf("... Done\n");
	/* ====================================================================================== */
	/* set up shared memory */
	retcode = setup_shm();
	handle_error(retcode, "Shared Memory could not be created.\n",
			PROCESS_EXIT);
	//struct shm_data *shm_data = (struct data *) shmat(shm_id, NULL, 0);

	char *shm_start = shmat(shm_id, NULL, 0);
	printf("... Done\n");

	/* init shared memory control*/
	retcode = initshm(shm_start);
	handle_error(retcode, "Could not create Shared Memory Control Set...\n",
			PROCESS_EXIT);

	/*n for testing memory control */
	printf("\n");
	int filesize = 1056;
	printf("Want to write filename with size=%i to shm\n", filesize);
	char * testfilename = "Test.txt";
	printf("Adress of shm Place to check is %x\n", shm_ctr);
	struct shm_ctr_struct *place = find_shm_place(shm_ctr, filesize);
	printf("Checked a good address is:  %x\n", place);

	if (place == FALSE) {
		printf(
				"0 is not valid. So there is no good place to write the file into... Trying no to devide the Shared Memory...\n");
		int block_size_needed = round_up_int(filesize);
		retcode = devide(shm_ctr, block_size_needed);
		handle_error(retcode,
				"Could not devide the shared memory for the needed size...\n",
				PROCESS_EXIT);

	} else {
		/* good place found */
//int b = TOT_SHM_SIZE;
//printf("b=%i",55);
//int a = (int) place->shm_size;
//printf("a = %i\t b=%i",a,b);
		if ((&(place->filedata) + place->shm_size)
				< (&(shm_ctr->filedata) + TOT_SHM_SIZE)) {
			printf("Inside of checking if return of place is valid...\n");
			place->isfree = FALSE;
			place->filename = testfilename;
		}

	}

	print_all_shm_blocks(shm_ctr);

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
#define DEBUG
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

void check_client_command() {

}

void breakCharArrayInWords(char *recMessage[], char squareBuffer[]) {
	/* break now the received Message into a string array where the sign " " breaks words */
	char breaksign[2] = " ";
	char *token;

	/* get the first token */
	token = strtok(squareBuffer, breaksign);
	int count = 0;
	/* walk through other tokens */
	while (token != NULL) {
		printf(" %s\n", token);
		recMessage[count] = token;

		//  strncpy(recMessage[count],token, 5);
		token = strtok(NULL, breaksign);
		//printf("Part of rec Msg: %s\n", recMessage[count]);
		count++;
	}

}

void handle_tcp_client(int clntSocket) {
	char *recMessage[32];
	char squareBuffer[BUFSIZE]; /* Buffer for square string */
	int recvMsgSize; /* Size of received message */

	while (TRUE) {
		/* Receive message from client */
		printf("Waiting for reveicing message from Client.\n");
		recvMsgSize = recv(clntSocket, squareBuffer, BUFSIZE - 1, 0);
		handle_error(recvMsgSize, "recv() failed", PROCESS_EXIT);
		printf("Received message from Client %s: %s\n",
				inet_ntoa(squareClntAddr.sin_addr), squareBuffer);
		printf("Received Message Size = %i\n", recvMsgSize);

		breakCharArrayInWords(recMessage, squareBuffer);

		/* handle now message and test if size is ok */
		//char str[10] = (char) squareBuffer[0];
		char bla[10];
		strncpy(bla, squareBuffer, 4);
		bla[5] = '\0';
		//printf("TEST before ATOI\n");
		//int should = (int) atoi(bla);
		printf("Size should be: %s\t Size is: %i\n", bla, recvMsgSize);

		if (recvMsgSize < 3) {

			char msg[BUFSIZE] =
					"Wrong number of arguments. Please write [CREATE|DELETE] <filename>";
			//char tab2[1024];
			strncpy(squareBuffer, msg, sizeof(msg));
			squareBuffer[sizeof(msg) - 1] = '\0';

			//	squareBuffer = "HELLO WRONG TEST";
			ssize_t sentSize = send(clntSocket, squareBuffer,
					strlen(squareBuffer), 0);

			//int msg = 5874;
			/*char msg = "Wrong number of arguments. Please write [CREATE|DELETE] <filename>";
			 sprintf(squareBuffer, "%c", msg);
			 int sendMsgSize = strlen(squareBuffer);
			 ssize_t sentSize = send(clntSocket, squareBuffer, sendMsgSize, 0);*/
			break;
		}

		squareBuffer[recvMsgSize] = '\000'; // set End Termination at the end of the Buffer

		int x = 5;
		int y = x * x;
		sprintf(squareBuffer, "%12d", y);
		int sendMsgSize = strlen(squareBuffer);
		ssize_t sentSize = send(clntSocket, squareBuffer, sendMsgSize, 0);
		/*if (sentSize != recvMsgSize) {
		 die_with_error("send() failed");
		 }*/
		/* See if there is more data to receive in the next round...*/
	}

	close(clntSocket); /* Close client socket */
}


