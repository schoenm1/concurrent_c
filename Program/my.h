struct shm_ctr_struct {
	int shm_size; //size of shm-block
	int isfree; // indicates if block is free or not
	int isLast; //indicates the end of shared memory
	struct shm_ctr_struct *next;
	struct shm_ctr_struct *prev;
	char *filename;
	char *filedata; // just this pointer is a pointer to Shared memory
	pthread_rwlock_t  rwlockFile; //Read-write lock for file
};

/* struct for handling the client pthreads */
struct pthread_struct {
	pthread_t thread;
	struct pthread_struct *nextClient;
	int isLast; //indicates the end of shared memory
};

/* struct for creating new Client pthreads and the params */
struct client_param_struct {
	int clientSocket;
};
