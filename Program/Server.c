/*
 * File: 		Main.c
 * Author: 		Micha Schšnenberger
 * Modul:		Concurrent Programming in C
 *
 * Created:     07.04.2014
 * Project:		https://github.com/schoenm1/concurrent_c.git
 */

/* ------------------ How to use this Program ------------------
 1) ...
 2) ...
 3) ...


 ---------------------------------------------------------------*/
#define _XOPEN_SOURC
#include "Logs.h"
#define TOT_SHM_SIZE 65536
#define MIM_SHM_BLOCK_SIZE 4
#define MAX_FILE_LENGTH 1500
#define MAX_WORD_SIZE 256
#define MUTEXSIZE 10
#include <arpa/inet.h>  /* for sockaddr_in, inet_addr() and inet_ntoa() */
#include <errno.h>
#include <math.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
#include <sys/socket.h> /* for socket(), bind(), recv, send(), and connect() */
#include <sys/types.h>
#include <unistd.h>     /* for close() */
#include <stdarg.h>
#include <string.h>
#include <itskylib.h>
#include <time.h>
#include "mystructs.h" // global structs
#include "shm_control.c" // global structs
#include "pthread_control.c" // control of Pthreads
#include "myfunctions.c"
#include <pthread.h> /* for pthreads */
#include "shm.c" /* for shared memory */

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
struct pthread_struct *myPThreadStruct;

/* all global variables for Arguments */
char _logLevel_arg[10] = LOGLEVEL_ARG
;
char _serverPort_arg[10] = SERVERPORT_ARG
;

/* forward function definition */
void ServerListen();
int setTCPServer();
void runClientCommand();
int setup_shm();
void my_handler();
int initshm();
int setLogLevel();
void breakCharArrayInWords();
void* handle_tcp_client();
int pthread_create(pthread_t * __restrict, const pthread_attr_t * __restrict, void *(*)(void *), void * __restrict);

/* global vars for TCP-Server */
int servSock; /* Socket descriptor for server */
int clientSocket; /* Socket descriptor for client */
struct sockaddr_in squareServAddr; /* Local address */
struct sockaddr_in ClientSocketAddress; /* Client address */
unsigned short ServerPort; /* Server port */
unsigned int client_address_len; /* Length of client address data structure */

struct shm_ctr_struct *shm_ctr;

struct validArgs {
	int isSet;
	char arg[10];
};

struct validArgs validArguments[5];

#include "valid-args.h"

int setup_shm() {
	/* set up shared Memory */
	printf("Setting up shared Memory ...");

	/* create REF File, if it not exists */
	remove(REF_FILE);
	create_if_missing(REF_FILE, S_IRUSR | S_IWUSR);

	/*create shm 'unique' key */
	key_t shm_key = ftok(REF_FILE, 1);
	if (shm_key < 0) {
		handle_error(-1, "ftok failed", NO_EXIT);
	}
	create_shm(shm_key, "create", "shmget failed", IPC_CREAT | IPC_EXCL);
	shm_id = create_shm(shm_key, "create", "shmget failed", 0);
	return 1;
}

void my_handler(int signo) {
	int retcode;
	if (signo == SIGUSR1) {
		LOG_TRACE(LOG_NOTICE, "Received other than SIGINT. Server is cleaning up shared memory and is going to close...");
		retcode = joiningAllPThreads(myPThreadStruct);
		handle_error(retcode, "Could not joining all PThreads", PROCESS_EXIT);
		/* clean up shared memory */
		cleanup(shm_id);
	} else {
		LOG_TRACE(LOG_NOTICE, "Received SIGINT. Server is cleaning up shared memory and is going to close...");
		retcode = joiningAllPThreads(myPThreadStruct);
		handle_error(retcode, "Could not joining all PThreads", PROCESS_EXIT);
		if (retcode == 0)
			LOG_TRACE(LOG_NOTICE, "Successfully joined all Client PThreads");
		/* clean up shared memory */
		cleanup(shm_id);
		exit(1);
	}
}

/* initializing Shared Memory Control Set*/
int initshm(char *shm_start) {
	LOG_TRACE(LOG_INFORMATIONAL, "Creating shm Control Set ...");
	shm_ctr = malloc(sizeof(struct shm_ctr_struct));
	shm_ctr->shm_size = TOT_SHM_SIZE;
	shm_ctr->isfree = TRUE;
	shm_ctr->isLast = TRUE;
	shm_ctr->next = shm_ctr;
	shm_ctr->prev = shm_ctr;
	shm_ctr->filename = "NULL";
	shm_ctr->filedata = shm_start;
	LOG_TRACE(LOG_INFORMATIONAL, "... Done\n");
	LOG_TRACE(LOG_INFORMATIONAL, "Shared Memory ID = %i\n", shm_id);
	LOG_TRACE(LOG_INFORMATIONAL, "Filename is: %s\n", shm_ctr->filename);
	LOG_TRACE(LOG_INFORMATIONAL, "Shared Memory Start location = %p\n", &(shm_ctr->filedata));
	return TRUE;
}

/* just exit the PThread */
void* endPThread(void* parameters) {
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	int retcode;
	LOG_TRACE(LOG_INFORMATIONAL, "\nPT: Server started. Is now initializing the setup...\n");

	signal(SIGINT, my_handler);

	LOG_TRACE(LOG_INFORMATIONAL, "PT: Setting up the valid arguments . . .");
	setValidServerArguments(); //setting up all valid arguments
	LOG_TRACE(LOG_INFORMATIONAL, ". . . Done\n");

	/* setup shared memory */
	retcode = setup_shm();
	handle_error(retcode, "PT: Shared Memory could not be created.\n", PROCESS_EXIT);

	char *shm_start = shmat(shm_id, NULL, 0);
	LOG_TRACE(LOG_INFORMATIONAL, "... Done\n");

	/* init shared memory control struct*/
	retcode = initshm(shm_start);
	handle_error(retcode, "PT: Could not create Shared Memory Control Set...\n", PROCESS_EXIT);

	/* init struct for PThread handling Clients */
	myPThreadStruct = (struct pthread_struct *) malloc(sizeof(struct pthread_struct));
	myPThreadStruct->isLast = 1;
	myPThreadStruct->nextClient = myPThreadStruct;
	pthread_t _firstPThread;
	pthread_create(&_firstPThread, NULL, &endPThread, NULL);

	/* Testfile for testing memory control */
	printf("\n");
	int filesize = 1056;
	LOG_TRACE(LOG_DEBUG, "Want to write filename with size=%i to shm\n", filesize);
	char * testfilename = "Test.txt";
	LOG_TRACE(LOG_DEBUG, "Adress of shm Place to check is %p\n", shm_ctr);
	struct shm_ctr_struct *place = find_shm_place(shm_ctr, filesize);
	LOG_TRACE(LOG_DEBUG, "Checked a good address is:  %p\n", place);

	/*if there is no good place found, devide shm blocks */
	if (place == FALSE) {
		LOG_TRACE(LOG_INFORMATIONAL, "0 here is no good place to write the file into... Trying no to devide the Shared Memory...");

		int block_size_needed = round_up_int(filesize);
		retcode = devide(shm_ctr, block_size_needed);
		handle_error(retcode, "Could not devide the shared memory for the needed size...\n", PROCESS_EXIT);
		place = find_shm_place(shm_ctr, filesize);
	}

	/* if address of founded place + size of place < than*/
	LOG_TRACE(LOG_DEBUG, "Address of place = %p\n", place);
	if (!place == 0) {
		printf("Inside of checking if return of place is valid...\n");
		place->isfree = FALSE;
		place->filename = testfilename;
		place->filedata = shm_start;
		shm_start = "Dies ist der Text des Testfiles.\nEs ist nicht einfach zu Programmieren.";

	}

	/* if no arguments is chosen, output the usage of the Server */
	if (argc == 1) {
		usage();
	} else {
		printf("Verify valid arguments ...\n");
		initValidServerArguments(argc, argv);
	}

	if (validArguments[1].isSet == 0) {
		printf("There was no argument for the Server-Port. It will no be set to default = 7000\n");
		ServerPort = 7000;
		validArguments[1].isSet = 1;

	} else {

	}

	retcode = setTCPServer();
	handle_error(retcode, "TCP Server settings could not be established!\n", PROCESS_EXIT);
	ServerListen();

	/* clean up shared memory */
	cleanup(shm_id);
}

int setTCPServer() {
	int retcode;
	printf("PT: Set up TCP-Server settings ...\n");

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
	retcode = bind(servSock, (struct sockaddr *) &squareServAddr, sizeof(squareServAddr));
	handle_error(retcode, "PT: bind() failed", PROCESS_EXIT);

	/* Mark the socket so it will listen for incoming connections */
	retcode = listen(servSock, MAXPENDING);
	handle_error(retcode, "PT: listen() failed", PROCESS_EXIT);

	return 1;
}

void ServerListen() {
	LOG_TRACE(LOG_INFORMATIONAL, "PT: Server is now going to Listening Mode for Clients.");
	LOG_TRACE(LOG_INFORMATIONAL, "PT: Client can connect to Server on Port %i", ServerPort);
	pthread_t _myPThread;
	int threadcounter = 0;
	/* Run forever */
	while (TRUE) {

		/* Set the size of the in-out parameter */
		client_address_len = sizeof(ClientSocketAddress);

		/* Wait for a client to connect */
		LOG_TRACE(LOG_INFORMATIONAL, "PT: Waiting for Client to connect...");
		clientSocket = accept(servSock, (struct sockaddr *) &ClientSocketAddress, &client_address_len);
		handle_error(clientSocket, "PT: accept() failed", 0);

		/* fill struct for pthread */
		struct client_param_struct cps;
		cps.clientSocket = clientSocket;
		cps.thread_count = threadcounter;

		/*handle the client and create per client a single thread */
		pthread_create(&_myPThread, NULL, &handle_tcp_client, &cps);
		addPThread(myPThreadStruct, _myPThread, threadcounter);

		/* clntSock is connected to a client! */
		LOG_TRACE(LOG_WARNING, "Handling Client %s", inet_ntoa(ClientSocketAddress.sin_addr));
		threadcounter++;
	}
	/* NOT REACHED: */
	exit(0);
}

void runClientCommand(char *recMessage[], char *command, int clntSocket, int thread_count) {
//LOG_TRACE(LOG_INFORMATIONAL, "Command from Client was: %s", command);
	char *sendtoClient = (char *) malloc(MAX_FILE_LENGTH);
	memset(sendtoClient, '\0', sizeof(sendtoClient));

	/* clear the last char of the received message. this is a newline which the server can not need */
	//recMessage[(strlen(recMessage) - 1)] = '\0';

	/* if Client want to exit, join PThread and Exit */
	if (strcmp(command, "EXIT") == 0) {
		LOG_TRACE(LOG_INFORMATIONAL, "Received EXIT from a Client");
		LOG_TRACE(LOG_INFORMATIONAL, "T%i: PThread is now going to exit", thread_count);
		pthread_exit(NULL);

	}

	/* CREATE command */
	if (strcmp(command, "CREATE") == 0) {
		LOG_TRACE(LOG_INFORMATIONAL, "PT: Will no try to create a new file...");
		char *filecontent = strdup(getFileContent(recMessage));
		LOG_TRACE(LOG_NOTICE, "Filesize = %i \t Content = %s", (int) strlen(filecontent), filecontent);
		char *filename = recMessage[2];

		char *returnvalue = malloc(sizeof(char) * MAX_FILE_LENGTH);
		returnvalue = writeNewFile(shm_ctr, filename, filecontent, strlen(filecontent));
		if (returnvalue > 0) {
			LOG_TRACE(LOG_INFORMATIONAL, "Sending message to Client: %s", returnvalue);
			send(clntSocket, returnvalue, strlen(returnvalue), 0);
		}
		free(returnvalue);
	}

	/* Reading File */
	if (strcmp(command, "READ") == 0) {
		char * returnvalue = readFile(shm_ctr, recMessage[2]);
		LOG_TRACE(LOG_INFORMATIONAL, "READ Command: Sending message to Client: %s", returnvalue);
		send(clntSocket, returnvalue, strlen(returnvalue), 0);
		free(returnvalue);

		/* unlock the read lock */
		//pthread_rwlock_rdlock(&(shm_ctr->rwlockFile));
		//free(returnvalue);
	}

	/* LIST * will return all blocks of the shared memory */
	if (strcmp(command, "LIST") == 0) {
		sendtoClient = get_all_shm_blocks(shm_ctr);
		send(clntSocket, sendtoClient, strlen(sendtoClient), 0);
		free(sendtoClient);
	}

	/* DELETE <filename>: DELETE Filename from memory */
	if (strcmp(command, "DELETE") == 0) {
		LOG_TRACE(LOG_NOTICE, "Client wants to DELETE a file.\n");
		int retcode;
		retcode = checkifexists(shm_ctr, recMessage[2]);

		/* if file does not exist, send message to Client */
		if (!retcode) {
			LOG_TRACE(LOG_NOTICE, "File \"%s\" should be deleted, but it does not exist", recMessage[2]);
			sendtoClient = getSingleString("File with the name \"%s\" does not exist!\n", recMessage[2]);
			send(clntSocket, sendtoClient, strlen(sendtoClient), 0);
			free(sendtoClient);
		}
		/* else delete the file */
		else {
			retcode = deleteFile(shm_ctr, recMessage[2]);
			printf("After deleting: SHM Block:\n");
			print_all_shm_blocks(shm_ctr);
			if (retcode) {
				sendtoClient = getSingleString("File with name \"%s\" was successfully deleted.", recMessage[2]);
				send(clntSocket, sendtoClient, strlen(sendtoClient), 0);
			}
			LOG_TRACE(LOG_INFORMATIONAL, "Will now try to combine free blocks...");
			retcode = combine(shm_ctr);
			/* repeat until there is no more deviding option */
			while (retcode) {
				retcode = combine(shm_ctr);
				print_all_shm_blocks(shm_ctr);
			}
		}
	}
	/* if nothing compared */
	else {
		LOG_TRACE(LOG_DEBUG, "No match. Send nothing to commit to client");
		send(clntSocket, "Nothing to commit", strlen("Nothing to commit"), 0);
	}
}

void* handle_tcp_client(void* parameters) {
	int istrue = 1;
	/* Cast the given parameter back to int ClntSocket  */
	struct client_param_struct* p = (struct client_param_struct*) parameters;
	int clntSocket = p->clientSocket;
	int thread_count = p->thread_count;
	p->thread_ID = (unsigned int) pthread_self();
	//LOG_TRACE(LOG_INFORMATIONAL, "T%i: New PThread created for Client.\t ID = %u", thread_count, (unsigned int) pthread_self());

	LOG_TRACE(LOG_INFORMATIONAL, "T%i: In new PThread: PThread ID = %ld", thread_count, (p->thread_ID));
	int retcode;
	char recBuffer[MAXRECWORDS]; /* Buffer for  string */
	int recvMsgSize; /* Size of received message */
	/* array to save the single words of the received message */
	char *recMessage[MAXRECWORDS];
	while (istrue) {
		/* reset Buffer for next transmission */
		memset(recBuffer, 0, sizeof(recBuffer));
		memset(recMessage, 0, sizeof(recMessage));
		/* Receive message from client */
		LOG_TRACE(LOG_INFORMATIONAL, "T%i: Waiting for reveicing message from Client.", thread_count);
		recvMsgSize = recv(clntSocket, recBuffer, BUFSIZE - 1, 0);
		handle_error(recvMsgSize, "recv() failed", NO_EXIT);
		if (recvMsgSize == 0) {
			istrue = 0;
			break;
		}
		LOG_TRACE(LOG_INFORMATIONAL, "T%i: Received message from Client %s: %s", thread_count, inet_ntoa(ClientSocketAddress.sin_addr),
				recBuffer);
		breakCharArrayInWords(recMessage, recBuffer);

		/* check is effective message is equal to expected message size */
		int effLength = (int) atoi(recMessage[0]);
		if (effLength == recvMsgSize) {
			//LOG_TRACE(LOG_NOTICE, "All OK!");

			/* check if 1st word of message is a valid command */
			retcode = getValidServerCommand(recMessage[1]);

			/* if command is valid */
			if (retcode) {
				LOG_TRACE(LOG_INFORMATIONAL, "T%i: It is a valid command: %s", thread_count, recMessage[1]);
				runClientCommand(recMessage, recMessage[1], clntSocket, thread_count);
			};
		}
		recBuffer[recvMsgSize] = '\000'; // set End Termination at the end of the Buffer
	}
	close(clntSocket); /* Close client socket */
	LOG_TRACE(LOG_INFORMATIONAL, "T%i: PThread now ending", thread_count);
}

/* set the chosen log level */
int setLogLevel(int logLevel) {
#ifndef DEBUG
#define  DEBUG
#endif
	LOGLEVEL = logLevel;
	return 1;
}
