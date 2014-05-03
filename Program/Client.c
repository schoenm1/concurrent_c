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
#include <sys/types.h>
#include <unistd.h>     /* for close() */

#include <itskylib.h>

#define BUFSIZE 1024   /* Size of receive buffer */

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
	char square_buffer[BUFSIZE]; /* Buffer for square string */
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

	while (TRUE) { /* Run forever */
		int num;
		char buff[BUFSIZE];
		char tmpsquare_buffer[BUFSIZE];
		printf("> Enter Command for Server:\n");
		fgets(tmpsquare_buffer, BUFSIZE - 2, stdin);

		/* check length of buffer and add int before it. This is the control for the server to check if he received the complete message */
		memset(square_buffer, 0, strlen(square_buffer));
		//clears the buffer

		char intlen[10];
		sprintf(intlen, "%9d", strlen(tmpsquare_buffer)+8+1 +1); //8 for int +1 for space, +1 for \0
		printf("Eingegebene Laenge (inkl. '0')= %s\t %i\n", intlen,
				strlen(tmpsquare_buffer));

		/* the message to send is: <length of following message><message> e.g. 5abcd (the '0' char will be added to <length of msg> */
		strcat(square_buffer, intlen);
		strcat(square_buffer, " ");
		printf("Size of square_buffer (just with len) = %i\n", strlen(square_buffer));
		strcat(square_buffer, tmpsquare_buffer);
printf("Size of square_buffer = %i\n", strlen(square_buffer));
		if ((send(sock, square_buffer, strlen(square_buffer), 0)) == -1) {
			fprintf(stderr, "Failure Sending Message\n");
			close(sock);
			exit(1);
		} else {
			printf("> Message being sent: %s\n", square_buffer);
			num = recv(sock, square_buffer, sizeof(square_buffer), 0);
			if (num <= 0) {
				printf("Either Connection Closed or Error\n");
				//Break from the While
				break;
			}

			buff[num] = '\0';
			printf("> Message Received From Server -  %s\n", square_buffer);
		}
	}
	close(sock);

	/* Receive the same string containing the square back from the server */
//  total_bytes_received = 0;
//	char squareBuffer[RCVBUFSIZE]; /* Buffer for square string */
	//printf("Received: ");                /* Setup to print the squared string */
//  char *ptr = square_buffer;
	// int sendMsgSize = RCVBUFSIZE;
	// prinft("Please give a command...\n");
//  int zahl;
//  scanf("%i", &zahl);
//	char squareBuffer[RCVBUFSIZE]; /* Buffer for square string */
//  ssize_t sentSize = send(sock, squareBuffer, sendMsgSize, 0);
	//while (total_bytes_received < input_string_len) {
	/* Receive up to the buffer size (minus 1 to leave space for
	 a null terminator) bytes from the sender */
	// bytes_received = recv(sock, ptr, RCVBUFSIZE - 1 - total_bytes_received, 0);
	//  if (bytes_received <= 0) {
	//    die_with_error("recv() failed or connection closed prematurely");
	//  }
	//   total_bytes_received += bytes_received;   /* Keep tally of total bytes */
	//   ptr += bytes_received;
	//   square_buffer[bytes_received] = '\0';  /* Terminate the string! */
	// }
	//int y = atoi(square_buffer);
	//printf("x=%d y=x*x=%d\n", x, y);    /* Print the result and a final linefeed */
	// }
	// close(sock);
	// exit(0);
}

