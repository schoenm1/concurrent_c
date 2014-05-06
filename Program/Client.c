/*
 * File: 		Client.c
 * Author: 		Micha Schšnenberger
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


#include <itskylib.h>

#define BUFSIZE 65535   /* Size of receive buffer */

unsigned short int send_length;

void usage(const char *argv0, const char *msg) {
	if (msg != NULL && strlen(msg) > 0) {
		printf("%s\n\n", msg);
	}
	printf("Usage\n\n");
	printf("%s <Server IP> <number> [<Port>]\n", argv0);
	exit(1);
}

int main(int argc, char *argv[]) {

	int retcode;

	if (is_help_requested(argc, argv)) {
		usage(argv[0], "");
	}

	int sock; /* Socket descriptor */
	struct sockaddr_in server_address; /* Square server address */
	unsigned short server_port; /* Square server port */
	char *server_ip; /* Server IP address (dotted quad) */
	// char input_string[13];              /* String to send to square server */
	char recbuffer[BUFSIZE]; /* Buffer for string */
	char sendbuffer[BUFSIZE]; /* Buffer for string */

	//unsigned int input_string_len; /* Length of string to square */
	//int bytes_received;//total_bytes_received;
	/* Bytes read in single recv()
	 and total bytes read */

	if (argc < 2 || argc > 3) { /* Test for correct number of arguments */
		usage(argv[0], "wrong number of arguments");
	}

	server_ip = argv[1]; /* First arg: server IP address (dotted quad) */
	//int x = atoi(argv[2]);         /* Second arg: string to square */
	// printf("Server Server-Port to %i\n",x);
	// sprintf(input_string, "%12d", x); /* make sure no non-numeric characters are around */

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
	retcode = connect(sock, (struct sockaddr *) &server_address,
			sizeof(server_address));
	handle_error(retcode, "connect() to Server failed", PROCESS_EXIT);

//  input_string_len = strlen(input_string);          /* Determine input length */

	/* Send the string to the server */
	/*  int count = send(sock, input_string, input_string_len, 0);
	 if (count != input_string_len) {
	 die_with_error("send() sent a different number of bytes than expected");
	 }*/


	/* Run forever */
	while (TRUE) {
		int num;
	//	char recbuff[BUFSIZE];
	//	char sendbuff[BUFSIZE];
		char tmpsquare_buffer[BUFSIZE];
		printf("> Enter Command for Server:\n");
		fgets(tmpsquare_buffer, BUFSIZE - 2, stdin);
		/* check length of buffer and add int before it. This is the control for the server to check if he received the complete message */
		/* clears the buffer for next transmission */
		memset(recbuffer, 0, strlen(recbuffer));
		memset(sendbuffer, 0, strlen(sendbuffer));
		recbuffer[0] = '\0';

		char intlen[10];
		/* 8 for int +1 for space, +1 for \0 */
		sprintf(intlen, "%9d", strlen(tmpsquare_buffer)+8+1 +1);
		//printf("Eingegebene Laenge (inkl. '0')= %s\t %i\n", intlen,strlen(tmpsquare_buffer));

		/* the message to send is: <length of following message><message> e.g. 5abcd (the '0' char will be added to <length of msg> */
		strcat(sendbuffer, intlen);
		strcat(sendbuffer, " ");
		printf("Size of square_buffer (just with len) = %i\n",strlen(sendbuffer));
		strcat(sendbuffer, tmpsquare_buffer);
		//printf("Size of square_buffer = %i\n", strlen(sendbuffer));

//if (strlen(sendbuffer)<10){break;}

		if ((send(sock, sendbuffer, strlen(sendbuffer), 0)) == -1) {
			fprintf(stderr, "Failure Sending Message\n");
			//close(sock);
			//exit(1);


		} else {
			printf("> Message being sent: %s\n", sendbuffer);
			recbuffer[0] = '\0';
			recv(sock, recbuffer, sizeof(recbuffer), 0);

		/*	if (num <= 0) {
				printf("Either Connection Closed or Error\n");
				//Break from the While
				break;
			}*/

			//sendbuffer[num] = '\0';
			printf("> Message Received From Server\n%s", recbuffer);
		}
	}
	close(sock);

}

