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

#define RCVBUFSIZE 128   /* Size of receive buffer */
#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define SERVERPORT_ARG "-p";
#define LOGLEVEL_ARG "-l";
#define SERVERNAME "Server";
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
	strncpy(validArguments[0].arg, _logLevel,strlen(_logLevel));

	/* init Server-Port */
	validArguments[1].isSet = 0;
	char _serverPort[5] = SERVERPORT_ARG;
	strncpy(validArguments[1].arg, _serverPort,strlen(_serverPort));
}

setTCPServer();

int main(int argc, char *argv[]) {
	int retcode;

	printf("\nServer started. Is now initializing the setup...\n");
	/* set up default Log-Level */
	retcode = setLogLevel(4);
	handle_error(retcode, "LogLevel could not be set.\n", PROCESS_EXIT);
	printf("... Done\n");

#ifdef LOGLEVEL_EMERGENCY
	printf("afasjfaisofh uidashfuiasdhuifahui ofh uiasdh fuiasdh uifohauiofh asuioh fuioasio\n");
#endif

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

	if (validArguments[1].isSet == 0){
		printf("There was no argument for the Server-Port. It will no be set to default = 7000\n");
		ServerPort = 7000;
		validArguments[1].isSet = 1;

	}


	retcode = setTCPServer();
	handle_error(retcode, "TCP Server settings could not be established!\n", PROCESS_EXIT);
	ServerListen();


}

int setTCPServer() {
	int retcode;
	printf("Set up TCP-Server settings ...\n");
	//printf("LOGLEVEL_DEBUG = %i",LOGLEVEL_DEBUG);

	/* Create socket for incoming connections */
	servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	handle_error(servSock, "socket() failed", PROCESS_EXIT);

	/* Construct local address structure */
	memset(&squareServAddr, 0, sizeof(squareServAddr)); /* Zero out structure */
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


void ServerListen(){
printf("Server is now going to Listening Mode for Clients.\n");
printf("Client can connect to Server on Port %i\n",ServerPort);
	while (TRUE) { /* Run forever */
	    /* Set the size of the in-out parameter */
	    clntAddrLen = sizeof(squareClntAddr);

	    /* Wait for a client to connect */
	    clntSock = accept(servSock, (struct sockaddr *) &squareClntAddr, &clntAddrLen);
	    handle_error(clntSock, "accept() failed", PROCESS_EXIT);

	    /* clntSock is connected to a client! */

	    printf("Handling client %s\n", inet_ntoa(squareClntAddr.sin_addr));
	    handle_tcp_client(clntSock);
	  }
	  /* NOT REACHED: */
	  exit(0);
}


void handle_tcp_client(int clntSocket) {
  char squareBuffer[RCVBUFSIZE];      /* Buffer for square string */
  int recvMsgSize;                    /* Size of received message */

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
    int y = x*x;
    sprintf(squareBuffer, "%12d", y);
    int sendMsgSize = strlen(squareBuffer);
    ssize_t sentSize = send(clntSocket, squareBuffer, sendMsgSize, 0);
    if (sentSize != recvMsgSize) {
      die_with_error("send() failed");
    }
    /* See if there is more data to receive in the next round...*/
  }

  close(clntSocket);    /* Close client socket */
}






