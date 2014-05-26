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
#include "Log-Level.h"
#define TOT_SHM_SIZE 65536
#define MIM_SHM_BLOCK_SIZE 4
#define MAX_FILE_LENGTH 1500
#define MAX_WORD_SIZE 256
#include <arpa/inet.h>  /* for sockaddr_in, inet_addr() and inet_ntoa() */
#include <errno.h>
#include<math.h>
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
#include "my.h" // global structs
#include "shm_control.c" // global structs
#include "myfunctions.c"
#include <pthread.h> /* for pthreads */
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
pthread_t threadlist[100];

/* all global variables for Arguments */
char _logLevel_arg[10] = LOGLEVEL_ARG
;
char _serverPort_arg[10] = SERVERPORT_ARG
;

/* fucntion prototypes */
void ServerListen();
int setTCPServer();
void runClientCommand();
int setup_shm();
void my_handler();
int initshm();
int setLogLevel();
void breakCharArrayInWords();
void* handle_tcp_client();

/* global vars for TCP-Server */
int servSock; /* Socket descriptor for server */
int clientSocket; /* Socket descriptor for client */
struct sockaddr_in squareServAddr; /* Local address */
struct sockaddr_in ClientSocketAddress; /* Client address */
unsigned short ServerPort; /* Server port */
unsigned int client_address_len; /* Length of client address data structure */

struct shm_ctr_struct *shm_ctr;
//char *default_Filename = "NULL";

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
		printf("Received other than SIGINT. Server is cleaning up shared memory and is going to close...\n");
		joinAllPthreads();
		/* clean up shared memory */
		cleanup(shm_id);
	} else {
		printf("Received SIGINT. Server is cleaning up shared memory and is going to close...\n");
		joinAllPthreads();
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

int main(int argc, char *argv[]) {
	int retcode;
	LOG_TRACE(LOG_INFORMATIONAL, "\nServer started. Is now initializing the setup...\n");

	signal(SIGINT, my_handler);

	LOG_TRACE(LOG_INFORMATIONAL, "Setting up the valid arguments...");
	setValidServerArguments(); //setting up all valid arguments
	LOG_TRACE(LOG_INFORMATIONAL, "... Done\n");

	retcode = setup_shm();
	handle_error(retcode, "Shared Memory could not be created.\n", PROCESS_EXIT);

	char *shm_start = shmat(shm_id, NULL, 0);
	LOG_TRACE(LOG_INFORMATIONAL, "... Done\n");

	/* init shared memory control*/
	retcode = initshm(shm_start);
	handle_error(retcode, "Could not create Shared Memory Control Set...\n", PROCESS_EXIT);

	/* Testfile for testing memory control */
	printf("\n");
	int filesize = 1056;
	printf("Want to write filename with size=%i to shm\n", filesize);
	char * testfilename = "Test.txt";
	printf("Adress of shm Place to check is %p\n", shm_ctr);
	struct shm_ctr_struct *place = find_shm_place(shm_ctr, filesize);
	printf("Checked a good address is:  %p\n", place);

	/*if there is no good place found, devide shm blocks */
	if (place == FALSE) {
		printf("0 is not valid. So there is no good place to write the file into... Trying no to devide the Shared Memory...\n");
		int block_size_needed = round_up_int(filesize);
		retcode = devide(shm_ctr, block_size_needed);
		handle_error(retcode, "Could not devide the shared memory for the needed size...\n", PROCESS_EXIT);
		place = find_shm_place(shm_ctr, filesize);
	}

// if address of founded place + size of place < than
	//	if ((&(place->filedata) + place->shm_size)< (&(shm_ctr->filedata) + TOT_SHM_SIZE)) {
	printf("Address of place = %p\n", place);
	if (!place == 0) {
		printf("Inside of checking if return of place is valid...\n");
		place->isfree = FALSE;
		place->filename = testfilename;
		place->filedata = shm_start;
		shm_start = "Dies ist der Text des Testfiles.\nEs ist nicht einfach zu Programmieren.";

	}
	//LOG_TRACE(LOG_INFORMATIONAL, "Will now output all existing shared memory blocks\n");
	//	print_all_shm_blocks(shm_ctr);

	/* ====================================================================================== */

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
	retcode = bind(servSock, (struct sockaddr *) &squareServAddr, sizeof(squareServAddr));
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
	LOG_TRACE(LOG_INFORMATIONAL, "Server is now going to Listening Mode for Clients.");
	LOG_TRACE(LOG_INFORMATIONAL, "Client can connect to Server on Port %i", ServerPort);
	int pthread_count = 0;

	/* Run forever */
	while (TRUE) {
		pthread_t newPThread;
		/* Set the size of the in-out parameter */
		client_address_len = sizeof(ClientSocketAddress);

		/* Wait for a client to connect */
		LOG_TRACE(LOG_INFORMATIONAL, "Waiting for Client for connecting...\n");
		clientSocket = accept(servSock, (struct sockaddr *) &ClientSocketAddress, &client_address_len);
		handle_error(clientSocket, "accept() failed", 0);

		/* fill struct for pthread */
		struct client_param_struct cps;
		cps.clientSocket = clientSocket;

		/*handle the client and create per client a single thread */
		pthread_create(&newPThread, NULL, &handle_tcp_client, &cps);
		threadlist[pthread_count]= newPThread;
		pthread_count++;
		LOG_TRACE(LOG_INFORMATIONAL, "Anzahl Clients = %i",pthread_count);



		/* clntSock is connected to a client! */
		LOG_TRACE(LOG_WARNING, "Handling Client %s", inet_ntoa(ClientSocketAddress.sin_addr));


		//handle_tcp_client(clientSocket);
	}
	/* NOT REACHED: */
	exit(0);
}

void joinAllPthreads(){



}





void runClientCommand(char *recMessage[], char *command, int clntSocket) {
	LOG_TRACE(LOG_INFORMATIONAL, "Command from Client was: %s", command);
	char *sendtoClient = (char *) malloc(MAX_FILE_LENGTH);
	memset(sendtoClient, '\0', sizeof(sendtoClient));

	/* CREATE command */
	if (strcmp(command, "CREATE") == 0) {

		LOG_TRACE(LOG_INFORMATIONAL, "Will no try to create a new file...");
		//printf("#0 = In creating new file\n");
		char * tmpchar = getFileContent(recMessage);
		printf("rec Message = %s\n", recMessage);

		char *filecontent = strdup(getFileContent(recMessage));
		//printf("#1 = In creating new file\n");
		char *filename = recMessage[2];
		//printf("#2 = In creating new file\n");
		LOG_TRACE(LOG_INFORMATIONAL, "Filesize = %i \t Content = %s", (int) strlen(filecontent), filecontent);

		char *returnvalue = malloc(sizeof(char) * 256);
		//printf("#3 = In creating new file\n");
		returnvalue = writeNewFile(shm_ctr, filename, filecontent, strlen(filecontent));
		LOG_TRACE(LOG_INFORMATIONAL, "Sending message to Client: %s", returnvalue);
		//printf("Will now send to the client Message mit Länge = %i: %s", strlen(returnvalue), returnvalue);
		//printf("#4 = In creating new file\n");
		send(clntSocket, returnvalue, strlen(returnvalue), 0);
		//printf("#5 = In creating new file\n");

		//free(filecontent);
		free(returnvalue);
	}

	/* Reading File */
	if (strcmp(command, "READ") == 0) {
		char * returnvalue = readFile(shm_ctr, recMessage[2]);
		send(clntSocket, returnvalue, strlen(returnvalue), 0);

	}

	if (strcmp(command, "LIST") == 0) {

		print_all_shm_blocks(shm_ctr);
		//char *sendtoClient = (char *) malloc(8192);
		sendtoClient = get_all_shm_blocks(shm_ctr);

		printf("Size of Send to Client = %i\n%s", strlen(sendtoClient), sendtoClient);

		send(clntSocket, sendtoClient, strlen(sendtoClient), 0);

		free(sendtoClient);
	}

	/* DELETE <filename>: DELETE Filename from memory */
	else if (strcmp(command, "DELETE") == 0) {
		printf("Client wants to DELETE a file.\n");
		int retcode;
		retcode = checkifexists(shm_ctr, recMessage[2]);

		/* if file does not exist, send message to Client */
		if (!retcode) {
			sendtoClient = getSingleString("File with the name \"%s\" does not exist!\n", recMessage[2]);
			send(clntSocket, sendtoClient, strlen(sendtoClient), 0);

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
			printf("Will now try to combine free blocks...\n");
			retcode = combine(shm_ctr);
			/* repeat until there is no more deviding option */

			print_all_shm_blocks(shm_ctr);
			while (retcode) {
				retcode = combine(shm_ctr);
				print_all_shm_blocks(shm_ctr);
			}
		}
	}
	/* if nothing compared */
	else {
		send(clntSocket, "Nothing to commit", strlen("Nothing to commit"), 0);
	}

}

void* handle_tcp_client(void* parameters) {
	int istrue = 1;
	LOG_TRACE(LOG_INFORMATIONAL, "New PThread created for Client.\t ID = %u", (unsigned int) pthread_self());
	/* Cast the given parameter back to int ClntSocket  */
	struct client_param_struct* p = (struct client_param_struct*) parameters;
	printf("clientSocket = %i", p->clientSocket);
	int clntSocket = p->clientSocket;
	printf("#3 In Handle Client\n");
	int retcode;
//char *recMessage[MAXRECWORDS]; /* array to save the single words of the received message */
	char recBuffer[MAXRECWORDS]; /* Buffer for  string */
	int recvMsgSize; /* Size of received message */
	/* array to save the single words of the received message */
	char *recMessage[MAXRECWORDS];

	while (istrue) {
		/* reset Buffer for next transmission */
		memset(recBuffer, 0, sizeof(recBuffer));
		memset(recMessage, 0, sizeof(recMessage));

		/* Receive message from client */
		printf("Waiting for reveicing message from Client.\n");
		recvMsgSize = recv(clntSocket, recBuffer, BUFSIZE - 1, 0);
		handle_error(recvMsgSize, "recv() failed", NO_EXIT);
		if (recvMsgSize == 0) {
			istrue = 0;
			break;
		}

		printf("Received message from Client %s: %s\n", inet_ntoa(ClientSocketAddress.sin_addr), recBuffer);
		printf("Received Message Size = %i\n", recvMsgSize);
		printf("rec Buffer = %s\n", recBuffer);

		printf("\nbefore break Array into Words...\n\n");
		//print_all_shm_blocks(shm_ctr);
		breakCharArrayInWords(recMessage, recBuffer);

#ifdef  DEBUG
		if (LOGLEVEL >= LOG_NOTICE) {
			LOG_TRACE(LOG_NOTICE, "Checking now if expected length of message is the effective length. If yes, transmission was ok.");
			//char *tmp;
			LOG_TRACE(LOG_NOTICE, "Size of transmitted message should be: %s\t Size is: %i", recMessage[0], recvMsgSize);
			/*printf("Size should be: %s\t Size is: %i\n", recMessage[0],
			 recvMsgSize);
			 */}
#endif
		/* check is effective message is equal to expected message size */
		int effLength = (int) atoi(recMessage[0]);
		if (effLength == recvMsgSize) {
#ifdef DEBUG
			if (LOGLEVEL >= LOG_NOTICE) {
				LOG_TRACE(LOG_NOTICE, "All OK!");
			}
#endif

			/* check if 1st word of message is a valid command */
			retcode = getValidServerCommand(recMessage[1]);

			LOG_TRACE(LOG_NOTICE, "Command is \"%s\" and has a length of %i", recMessage[1], strlen(recMessage[1]));

			/* if command is valid */
			if (retcode) {
				LOG_TRACE(LOG_INFORMATIONAL, "It is a valid command: %s", recMessage[1]);
				LOG_TRACE(LOG_INFORMATIONAL, "Führe nun Befehl aus: %s", recMessage[1]);
				runClientCommand(recMessage, recMessage[1], clntSocket);
			};

		}

		recBuffer[recvMsgSize] = '\000'; // set End Termination at the end of the Buffer

	}

	close(clntSocket); /* Close client socket */
}

void breakCharArrayInWords(char *recMessage[], char *recBuffer[]) {
	//printf("## Now in breakCharArrayinWords ##\n");

	/* break now the received Message into a string array where the sign " " breaks words */
	char breaksign[] = " ";
	char *token = malloc(sizeof(char) * MAX_FILE_LENGTH);
	int count = 0;
	//printf("## 2 Now in breakCharArrayinWords ##\n");

	/* get the first token */
	token = strtok(recBuffer, breaksign);
	//printf("## 3 Now in breakCharArrayinWords ##\n");
	//printf("complete message = %s", recBuffer);
	//printf("%i: Token = %s and Size of token = %i\n", count, token, (int) strlen(token));
	recMessage[count] = token;
	//printf("recMessage[%i]= %s\n", count, recMessage[count]);
	count++;

	/* walk through other tokens */
	while (token != NULL) {
		token = strtok(NULL, breaksign);
		recMessage[count] = malloc(sizeof(char) * MAX_WORD_SIZE);
		recMessage[count] = token;
		//printf("in while: recMessage[%i]= %s\n", count, recMessage[count]);
		count++;
	}
}
