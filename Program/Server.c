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

//#include "Log-Level.h"
#define TOT_SHM_SIZE 65536
#define MIM_SHM_BLOCK_SIZE 4
#include <arpa/inet.h>  /* for sockaddr_in, inet_addr() and inet_ntoa() */
#include <errno.h>
#include<math.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
#include <string.h>     /* for memset() and ... */
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




#define RCVBUFSIZE 128   /* Size of receive buffer */
#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define SERVERPORT_ARG "-p";
#define LOGLEVEL_ARG "-l";
#define SERVERNAME "Server";
int _MAX_LENGTH_ARG = 5; // defines the maximum Length of arguments. e.g. "-l"

/* all global variables for Arguments */
char _logLevel_arg[10] = LOGLEVEL_ARG;
char _serverPort_arg[10] = SERVERPORT_ARG;

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

void setValidArguments() {

	/* init LOGLEVEL */
	validArguments[0].isSet = 0;
	char _logLevel[5] = LOGLEVEL_ARG;
	strncpy(validArguments[0].arg, _logLevel, strlen(_logLevel));

	/* init Server-Port */
	validArguments[1].isSet = 0;
	char _serverPort[5] = SERVERPORT_ARG;
	strncpy(validArguments[1].arg, _serverPort, strlen(_serverPort));
}

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
	 printf("Received other than SIGINT. Server is cleaning up shared memory and is going to close...\n");
	  /* clean up shared memory */
	  	  	cleanup(shm_id);
  } else {
	  printf("Received SIGINT. Server is cleaning up shared memory and is going to close...\n");
	  /* clean up shared memory */
	  	cleanup(shm_id);
    exit(1);
  }
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
	printf("Shared Memory ID = %i\n",shm_id);
	printf("Filename is: %c\n",shm_ctr->filedata);
	printf("Shared Memory Start location = %p\n",&(shm_ctr->filedata));


/*n for testing memory control */
printf("\n");
int filesize = 1056;
printf("Want to write filename with size=%i to shm\n",filesize);
char * testfilename = "Test.txt";
printf("Adress of shm Place to check is %x\n",shm_ctr);
struct shm_ctr_struct *place = find_shm_place(shm_ctr, filesize);
printf("Checked a good address is:  %x\n",place);



if (place == FALSE){
	printf("0 is not valid. So there is no good place to write the file into... Trying no to devide the Shared Memory...\n");
int block_size_needed = round_up_int(filesize);
retcode = devide(shm_ctr, block_size_needed);
handle_error(retcode, "Could not devide the shared memory for the needed size...\n",
			PROCESS_EXIT);

}
else{
/* good place found */
int b = TOT_SHM_SIZE;
//printf("b=%i",55);
int a = (int) place->shm_size;
//printf("a = %i\t b=%i",a,b);
if ((&(place->filedata) + place->shm_size) < (&(shm_ctr->filedata)+TOT_SHM_SIZE)){
	printf("Inside of checking if return of place is valid...\n");
place->isfree = FALSE;
place->filename = testfilename;
}

}


print_all_shm_blocks(shm_ctr);



	/* ====================================================================================== */

	printf("Setting up the valid arguments...");

	setValidArguments(); //setting up all valid arguments
	printf("... Done\n");

	/* if no arguments is chosen, output the usage of the Server */
	if (argc == 1) {
		usage();
	} else {
		printf("Verify valid arguments ...\n");
		initValidArguments(argc, argv);
	}

	if (validArguments[1].isSet == 0) {
		printf(
				"There was no argument for the Server-Port. It will no be set to default = 7000\n");
		ServerPort = 7000;
		validArguments[1].isSet = 1;

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

void ServerListen() {
	printf("Server is now going to Listening Mode for Clients.\n");
	printf("Client can connect to Server on Port %i\n", ServerPort);
	while (TRUE) { /* Run forever */
		/* Set the size of the in-out parameter */
		clntAddrLen = sizeof(squareClntAddr);

		/* Wait for a client to connect */
		clntSock = accept(servSock, (struct sockaddr *) &squareClntAddr,
				&clntAddrLen);
		handle_error(clntSock, "accept() failed", PROCESS_EXIT);

		/* clntSock is connected to a client! */

		printf("Handling client %s\n", inet_ntoa(squareClntAddr.sin_addr));
		handle_tcp_client(clntSock);
	}
	/* NOT REACHED: */
	exit(0);
}

void handle_tcp_client(int clntSocket) {
	char squareBuffer[RCVBUFSIZE]; /* Buffer for square string */
	int recvMsgSize; /* Size of received message */

	while (TRUE) {
		/* Receive message from client */
		recvMsgSize = recv(clntSocket, squareBuffer, RCVBUFSIZE - 1, 0);
		handle_error(recvMsgSize, "recv() failed", PROCESS_EXIT);

		if (recvMsgSize == 0) {
			/* zero indicates end of transmission */
			break;
		}
		squareBuffer[recvMsgSize] = '\000';
		/* Send received string and receive again until end of transmission */
		/* Square message and send it back to client */
		int x = atoi(squareBuffer);
		int y = x * x;
		sprintf(squareBuffer, "%12d", y);
		int sendMsgSize = strlen(squareBuffer);
		ssize_t sentSize = send(clntSocket, squareBuffer, sendMsgSize, 0);
		if (sentSize != recvMsgSize) {
			die_with_error("send() failed");
		}
		/* See if there is more data to receive in the next round...*/
	}

	close(clntSocket); /* Close client socket */
}

