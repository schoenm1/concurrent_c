/*
 * File: 		pthread_control.c
 * Author: 		Micha Schšnenberger
 * Modul:		Concurrent Programming in C
 *
 * Created:     26.05.2014
 * Project:		https://github.com/schoenm1/concurrent_c.git
 */

struct client_ctl_struct* getClient(struct client_ctl_struct *pthread_struct, int id) {
	//int phtread_id = id;

	return NULL;

}

/* waiting for joining all the PThreads  */
int joinPThread(struct pthread_struct *_thisPThread) {
	printf("1-Now in function joinPthread\n");
	printf("PThread isLast = %i", (_thisPThread->thread));

	while (!_thisPThread->isLast) {
		printf("2-Now in function joinPthread\n");
		pthread_join(_thisPThread, NULL);
		_thisPThread = _thisPThread->nextClient;
		printf("3-Now in function joinPthread\n");
	}

	printf("4-Now in function joinPthread\n");
	_thisPThread = _thisPThread->nextClient;

	pthread_join(_thisPThread, NULL);

	return TRUE;
}

/* add the PThread to the PThread Client list, if a new Client is connecting  */
int addPThread(struct pthread_struct *_pthread, pthread_t pThread) {
	int retcode = FALSE;
	printf("Now in function addPThread ");
	//struct pthread_struct *_pthread = pthread_struct;
	printf(" 1 ");
	/* if _pthread is Last, add new PThread at the end of the list */
	if (_pthread->isLast) {
		printf(" 2 ");
		struct pthread_struct *_newPThread = (struct pthread_struct *) malloc(sizeof(struct pthread_struct));
		printf(" 3 ");
		_pthread->nextClient = _newPThread;
		_pthread->isLast = FALSE;
		printf(" 4 ");
		_newPThread->thread = pThread;
		_newPThread->nextClient = _newPThread;
		_newPThread->isLast = TRUE;
	}
	printallPThreads();
		printf(" End of addPThread\n");
	return retcode;
}


void printallPThreads(struct pthread_struct *pthread_struct){
int count = 0;
while (pthread_struct->isLast){
	printf("PThread %i = %i", count, pthread_struct->thread);
		pthread_struct = pthread_struct->nextClient;
count++;
}
printf("PThread %i = %i", count, pthread_struct->thread);



}


/* delete the PThread from the PThread Client list, if Client disconnect */
int detachPThread(struct pthread_struct *pthread_struct, int id) {
	int phtread_id = id;

	return FALSE;
}

