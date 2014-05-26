/*
 * main.c
 *
 *  Created on: Apr 26, 2014
 *      Author: fish-guts
 */

#include "main.h"


/* our main server buffer */
char serverbuf[4096];
int quitting;

thread *threadlist = NULL;


void parse(void) {
	char source[1024];
	char command[1024];
	char buf[1024];
	char err[1024];
	char *pch;
	int ac;
	char **av;
	cmd *ic;
	strscpy(buf, serverbuf, sizeof(buf));
	if (*buf == ':') {
		pch = strpbrk(buf, " ");
		if (!pch)
			return;
		*pch = 0;
		while (isspace(*++pch))
			;
		strscpy(source, buf + 1, sizeof(source));
		strscpy(buf, pch, sizeof(buf));

	} else {
		*source = 0;
	}
	if (!*buf)
		return;
	pch = strpbrk(buf, " ");
	if (pch) {
		*pch = 0;
		while (isspace(*++pch))
			;
	} else
		pch = buf + strlen(buf);
	strscpy(command, buf, sizeof(command));
	ac = tokenize(pch, &av);
	if ((ic = find_cmd(command))) {
		if (ic->func)
			ic->func(source, ac, av);
	} else {
		fprintf(stderr, "Unknown command: %s\n", serverbuf);
		sprintf(buf,"Unknown Command: %s\n",serverbuf);
		send(client_sock,buf,sizeof(buf),0);
	}
	free(av);

}

void add_thread(thread *t) {
	t->next = threadlist;
	if (threadlist)
		threadlist->prev = t;
	threadlist = t;

}


void launch_app(char *argv[]) {
	if ((strcmp(argv[1], "start")) == 0)
		startup();
	else if ((strcmp(argv[1], "stop")) == 0)
		stop_server();
	else {
		fprintf(stderr,
				"Invalid Command: %s. Valid Commands are ./fileserver [start|stop]\n",
				argv[1]);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char* argv[]) {
	if (argc > 1)
		launch_app(argv);
	else {
		fprintf(stderr,
				"No argument supplied. Valid Argument are [start|stop]\n");
		exit(EXIT_SUCCESS);
	}
	return 0;
}

void print_start_msg(void) {
	fprintf(stderr, "###############################\n");
	fprintf(stderr, "Welcome to Severin'ŝ FileServer\n");
	fprintf(stderr, "###############################\n");
}

void stop_server(void) {
	exit(EXIT_SUCCESS);
}

void startup(void) {
	print_start_msg();

	// daemonize, not yet needed.
	/*	pid_t pid, sid;
	 if (getppid() == 1)
	 return;
	 pid = fork();
	 if (pid < 0)
	 exit(EXIT_FAILURE);
	 if (pid > 0)
	 exit(EXIT_SUCCESS);
	 umask(0);
	 sid = setsid();
	 if (sid < 0)
	 exit(EXIT_FAILURE);
	 int rc;
	 if ((chdir("/")) < 0)
	 exit(EXIT_FAILURE);
	 // done, now we need to open some sockets and wait for connections;
	 * */

	start_server();
}

void start_server(void) {
	int s, len, rc;
	int tid;
	long t;
	char buf[100000];

	struct sockaddr_in addr;
	struct sockaddr_in client;
	pthread_t client_thread;
	pthread_mutex_t mx;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	unsigned short port = PORT;

	// clear the struct
	memset((char*) &addr, 0, sizeof(addr));

	fprintf(stderr, "\n\nStarting server...");
	// let's set some values

	/* type of socket created in socket() */
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	/* 7000 is the port to use for connections */
	addr.sin_port = htons((unsigned short) PORT);

	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	/* bind the socket to the port specified above */

	if ((rc = bind(sock, (struct sockaddr *) &addr, sizeof(addr))) < 0) {
		fprintf(stderr, "Error binding address: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	} else {
		fprintf(stderr, "Bind Successful\n");
	}

	if ((listen(sock, serverbuf) < 0)) {
		fprintf(stderr, "Listen Failed\n");
		exit(EXIT_FAILURE);
	} else {
		fprintf(stderr, "Server started successfully, listening on port %d\n",
				PORT);
	}
	// the  main server loop
	while (!quitting) {
		len = sizeof(struct sockaddr_in);

		client_sock = accept(sock, (struct sockaddr *) &client, &len);
		if (client_sock < 0) {
			fprintf(stderr, "Accept failed\n");
		} else {
			/* This is the client process */

			thread *t = scalloc(sizeof(thread), 1);
			t->mx = &mx;
			t->t = pthread_self();
			pthread_mutex_init(&mx,NULL);
			pthread_mutex_lock(&mx);
			tid = pthread_create(&client_thread, NULL, doprocessing, &t);
			add_thread(t);
			if (tid) {
				fprintf(stderr, "Error creating thread: %d\n", tid);
			}
		}
	}
}

void *doprocessing(thread *t) {
	pthread_mutex_t *mx;
	pthread_mutex_t *mxq;
	int n,s;
	char buf[100000];
	bzero(buf, sizeof(buf));


	if (n < 0) {
		fprintf(stderr, "ERROR writing to socket");
		exit(1);
	}
	while (!quitting) {
		s = recv(client_sock, buf, sizeof(serverbuf), 0);
		if (s>0) {
			buf[s] = 0;
			// we use LF as a line breaker, its easier to parse the commands
			char *pch = strtok(buf, "\n");
			while (pch != NULL) {
				strcpy(serverbuf, pch);
				parse();
				serverbuf[s] = 0;
				pch = strtok(NULL, "\n");
			}
		} else {
			fprintf(stderr, "Client disconnected\n");
			close(client_sock);
			pthread_exit(t->t);
			pthread_mutex_unlock(&mxq);

		}
	}
}
