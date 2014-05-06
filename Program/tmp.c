/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * This file is inspired by
 * http://cs.baylor.edu/~donahoo/practical/CSockets/code/TCPEchoServer.c
 * and
 * http://cs.baylor.edu/~donahoo/practical/CSockets/code/HandleTCPClient.c
 */

#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() and inet_ntoa() */
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
#include <string.h>     /* for memset() and ... */
#include <sys/socket.h> /* for socket(), bind(), recv, send(), and connect() */
#include <sys/types.h>
#include <unistd.h>     /* for close() */
#include <pthread.h>

#include <itskylib.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */
#define MAXPENDING 5    /* Maximum outstanding connection requests */

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s <Server Port>\n", argv0);
  exit(1);
}

void *handle_tcp_client(void *client_socket_ptr);   /* TCP client handling function */

int main(int argc, char *argv[]) {

  int retcode;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  int server_socket;                    /* Socket descriptor for server */
  int client_socket;                    /* Socket descriptor for client */
  struct sockaddr_in server_address; /* Local address */
  struct sockaddr_in client_address; /* Client address */
  unsigned short server_port;     /* Server port */
  unsigned int client_address_len;            /* Length of client address data structure */

  if (argc != 2) {
    /* Test for correct number of arguments */
    usage(argv[0], "wrong number of arguments");
  }
  server_port = atoi(argv[1]);  /* First arg:  local port */

  /* Create socket for incoming connections */
  server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  handle_error(server_socket, "socket() failed", PROCESS_EXIT);

  /* Construct local address structure */
  memset(&server_address, 0, sizeof(server_address));   /* Zero out structure */
  server_address.sin_family = AF_INET;                /* Internet address family */
  server_address.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
  server_address.sin_port = htons(server_port);      /* Local port */

  /* Bind to the local address */
  retcode = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
  handle_error(retcode, "bind() failed", PROCESS_EXIT);

  /* Mark the socket so it will listen for incoming connections */
  retcode = listen(server_socket, MAXPENDING);
  handle_error(retcode, "listen() failed", PROCESS_EXIT);

  while (TRUE) { /* Run forever */
    /* Set the size of the in-out parameter */
    client_address_len = sizeof(client_address);

    /* Wait for a client to connect */
    client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_len);
    handle_error(client_socket, "accept() failed", PROCESS_EXIT);

    printf("Handling client %s\n", inet_ntoa(client_address.sin_addr));
    /* client_socket is connected to a client! */

    pthread_t thread;
    int *client_socket_ptr = (int *) malloc(sizeof(int));
    *client_socket_ptr = client_socket;
    pthread_create(&thread, NULL, handle_tcp_client, client_socket_ptr);
    pthread_detach(thread);
  }
  /* NOT REACHED: */
  exit(0);
}

void* handle_tcp_client(void *client_socket_ptr) {
  int client_socket = *((int *) client_socket_ptr);
  free(client_socket_ptr); /* malloc was made before starting this thread */
  char square_buffer[RCVBUFSIZE];      /* Buffer for square string */
  int recv_msg_size;                    /* Size of received message */

  while (TRUE) {
    /* Receive message from client */
    recv_msg_size = recv(client_socket, square_buffer, RCVBUFSIZE - 1, 0);
    handle_error(recv_msg_size, "recv() failed", PROCESS_EXIT);

    if (recv_msg_size == 0) {
      /* zero indicates end of transmission */
      break;
    }
    square_buffer[recv_msg_size] = '\000';
    /* Send received string and receive again until end of transmission */
    /* Square message and send it back to client */
    int x = atoi(square_buffer);
    int y = x*x;
    sprintf(square_buffer, "%12d", y);
    int send_msg_size = strlen(square_buffer);
    ssize_t sent_size = send(client_socket, square_buffer, send_msg_size, 0);
    if (sent_size != recv_msg_size) {
      die_with_error("send() failed");
    }
    /* See if there is more data to receive in the next round...*/
  }

  close(client_socket);    /* Close client socket */
  return NULL;
}
