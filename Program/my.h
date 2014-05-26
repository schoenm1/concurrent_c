struct shm_ctr_struct {
	int shm_size; //size of shm-block
	int isfree; // indicates if block is free or not
	int isLast; //indicates the end of shared memory
	struct shm_ctr_struct *next;
	struct shm_ctr_struct *prev;
	char *filename;
	char *filedata; // just this pointer is a pointer to Shared memory
};


/* struct for handling the client pthreads */
struct client_ctl_struct {
	pthread_t thread;
	struct client_ctl_struct *nextClient;
	int clntSocket;
};

/* struct for creating new Client pthreads and the params */
struct client_param_struct {
	int clientSocket;
};
