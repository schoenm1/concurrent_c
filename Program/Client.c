/*
 * File: 		Client.c
 * Author: 		Micha Schönenberger
 * Modul:		Concurrent Programming in C
 *
 * Created:     14.04.2014
 * Project:		https://github.com/schoenm1/concurrent_c.git
 */

/* ------------------ How to user this Program ------------------
 1) ...
 2) ...
 3) ...


 ---------------------------------------------------------------*/

#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
#include <string.h>     /* for memset() and ... */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <time.h>
#include <sys/types.h>
#include <unistd.h>     /* for close() */
#include <signal.h>
#include <itskylib.h>
#include <unistd.h>

#define BUFSIZE 65535   /* Size of receive buffer */
int sock; /* Socket descriptor */
struct sockaddr_in server_address; /* Square server address */
unsigned short server_port; /* Square server port */
char *server_ip; /* Server IP address (dotted quad) */

unsigned short int send_length;

/* forward declarations of functions */
void usage(const char *argv0, const char *msg);
void my_handler(int signo);
void clearBuffers(char sendbuffer[BUFSIZE], char recbuffer[BUFSIZE]);
void calcMsgToSend(char sendbuffer[BUFSIZE], char tmpsquare_buffer[BUFSIZE]);
void closeSocket();

/*
 * BEGIN OF Client.c
 */

/* shows the usage of the Client to connect to the Server */
void usage(const char *argv0, const char *msg) {
	if (msg != NULL && strlen(msg) > 0) {
		printf("%s\n\n", msg);
	}
	printf("Usage\n\n");
	printf("%s <Server IP> <number> [<Port>]\n", argv0);
	exit(1);
}

/* Handles the Signals which are received by the Client */
void my_handler(int signo) {
	if (signo == SIGTERM) {
		printf("Received and ignored SIGTERM.\n");
	} else if (signo == SIGINT) {
		printf("Received Ctrl-C. Send now Command to Server that Client exit.\n");
		char sendbuffer[BUFSIZE];
		calcMsgToSend(sendbuffer, "EXIT");
		send(sock, sendbuffer, strlen(sendbuffer), 0);
		usleep(100);
		exit(1);
	} else {
		printf("unknow Signal %d will be ignored\n", signo);
	}
}

int main(int argc, char *argv[]) {
	signal(SIGINT, my_handler);
	int retcode;

	if (is_help_requested(argc, argv)) {
		usage(argv[0], "");
	}

	char recbuffer[BUFSIZE]; /* Buffer for string */
	char sendbuffer[BUFSIZE]; /* Buffer for string */

	if (argc < 2 || argc > 3) { /* Test for correct number of arguments */
		usage(argv[0], "wrong number of arguments");
	}

	server_ip = argv[1]; /* First arg: server IP address (dotted quad) */

	if (argc == 3) {
		server_port = atoi(argv[2]); /* Use given port, if any */
	} else {
		server_port = 7000; /* 7000 is a free port */
	}

	/* Create a reliable, stream socket using TCP */
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	handle_error(sock, "socket() failed", PROCESS_EXIT);

	/* Construct the server address structure */
	memset(&server_address, 0, sizeof(server_address));
	/* Zero out structure */
	server_address.sin_family = AF_INET; /* Internet address family */
	server_address.sin_addr.s_addr = inet_addr(server_ip); /* Server IP address */
	server_address.sin_port = htons(server_port); /* Server port: htons host to network byte order */

	/* Establish the connection to the square server */
	retcode = connect(sock, (struct sockaddr *) &server_address, sizeof(server_address));
	handle_error(retcode, "connect() to Server failed", PROCESS_EXIT);

	/* Run forever */

	while (TRUE) {
		int num;
		char tmpsquare_buffer[BUFSIZE];
		printf("\n# Enter Command for Server: ");
		fgets(tmpsquare_buffer, BUFSIZE - 2, stdin);
		strcat(tmpsquare_buffer, " ");
		clearBuffers(sendbuffer, recbuffer);
		calcMsgToSend(sendbuffer, tmpsquare_buffer);

		if ((send(sock, sendbuffer, strlen(sendbuffer), 0)) == -1) {
			fprintf(stderr, "Failure Sending Message\n");

		} else {
			printf("# Message being sent: %s\n", sendbuffer);
			recbuffer[0] = '\0';
			recv(sock, recbuffer, sizeof(recbuffer), 0);

			printf("# Message from Server: %s\n", recbuffer);
		}
	}
	closeSocket();
}

/* clear the recbuffer and the sendbuffer */
void clearBuffers(char sendbuffer[BUFSIZE], char recbuffer[BUFSIZE]) {
	/* check length of buffer and add int before it. This is the control for the server to check if he received the complete message */
	/* clears the buffer for next transmission */
	memset(recbuffer, 0, strlen(recbuffer));
	memset(sendbuffer, 0, strlen(sendbuffer));
	recbuffer[0] = '\0';
}

/* here the message will be calculated. This is a short transmission protocol to ensure that all traffic was sent */
void calcMsgToSend(char sendbuffer[BUFSIZE], char tmpsquare_buffer[BUFSIZE]) {
	char intlen[10];
	/* 8 for int +1 for space, +1 for \0 */
	sprintf(intlen, "%9d", strlen(tmpsquare_buffer)+8+1 +1);

	/* the message to send is: <length of following message><message> e.g. 5abcd (the '0' char will be added to <length of msg> */
	strcat(sendbuffer, intlen);
	strcat(sendbuffer, " ");
	strcat(sendbuffer, tmpsquare_buffer);
}

void closeSocket() {
	close(sock);
}
