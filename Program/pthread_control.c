/*
 * File: 		pthread_control.c
 * Author: 		Micha Schšnenberger
 * Modul:		Concurrent Programming in C
 *
 * Created:     26.05.2014
 * Project:		https://github.com/schoenm1/concurrent_c.git
 */

/* forward function definitions */
int pthread_join(pthread_t, void **);
void printallPThreads(struct pthread_struct *pthread_struct);

struct client_ctl_struct* getClient(struct client_ctl_struct *pthread_struct, int id) {
	//int phtread_id = id;
	return NULL;
}

/*

 int deletePThrad(struct pthread_struct *pthread_struct) {
 int retcode = FALSE;
 struct pthread_struct *tmpThread = pthread_struct;

 int pthread_tryjoin_np(pthread_t thread, void **retval);

 int pthread_timedjoin_np(pthread_t thread, void **retval, const struct timespec *abstime);

 return retcode;

 }


 /* join all PThreads - if PThread is not ended, this function blocks the MainThread of the Server*/
int joiningAllPThreads(struct pthread_struct *myPThreadList) {
	unsigned long pThread_ID;
	struct pthread_struct *_myPThread = myPThreadList;
	struct pthread_struct *_nextPThread = _myPThread->nextClient;
	int retcode = FALSE;
	LOG_TRACE(LOG_DEBUG, "PT: In Joining PThreads()");
	printallPThreads(myPThreadList);

	/* joining all PThread except the last one */
	while (!_myPThread->isLast) {
		pThread_ID = _myPThread->thread;
		pthread_join(_myPThread->thread, NULL);
		LOG_TRACE(LOG_DEBUG, "PT: Successfully joined PThread %lu", pThread_ID);
		_myPThread = _nextPThread;
		_nextPThread = _nextPThread->nextClient;
	}

	/* joining the last PThread*/
	retcode = pthread_join(_myPThread->thread, NULL);
	LOG_TRACE(LOG_DEBUG, "PT: Successfully joined PThread %lu", pThread_ID);
	handle_error(retcode, "PT: Failing to join PThread", NO_EXIT);
	return retcode;
}
/* try to join possible PThreads */
/*int tryJoiningPThread(struct pthread_struct *_thisPThread) {
 int retcode;
 struct timespec ts;
 int s;
 if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
 // Handle error 	}

 ts.tv_sec += 1;

 s = pthread_timedjoin_np(_thisPThread, NULL, &ts);
 if (s != 0) {
 // Handle error
 }

 while (!_thisPThread->isLast) {
 pthread_tryjoin_np(_thisPThread);
 //		pthread_join(_thisPThread, NULL);
 //_thisPThread = _thisPThread->nextClient;
 }

 printf("4-Now in function joinPthread\n");
 _thisPThread = _thisPThread->nextClient;

 pthread_join(_thisPThread, NULL);

 return TRUE;
 }*/

/* add the PThread to the PThread Client list, if a new Client is connecting  */
int addPThread(struct pthread_struct *_pthreadStruct, pthread_t pThread, int counter) {
	int retcode = FALSE;
	LOG_TRACE(LOG_DEBUG, "PT: Now in function addPThread");
	struct pthread_struct *_tmpPThread = (struct pthread_struct *) malloc(sizeof(struct pthread_struct));
	/* if _pthread is Last, add new PThread at the end of the list */
	while (TRUE) {

		if (_pthreadStruct->isLast) {
			struct pthread_struct *_newPThread = (struct pthread_struct *) malloc(sizeof(struct pthread_struct));
			_pthreadStruct->nextClient = _newPThread;
			_newPThread->nextClient = _newPThread; //link next to himself
			_pthreadStruct->isLast = FALSE;
			_newPThread->isLast = TRUE;
			_newPThread->thread = pThread;
			_newPThread->PThread_count = counter;
			LOG_TRACE(LOG_INFORMATIONAL, "PT: Added PThread: ID= %ld\tPThread-No: %i\t isLast = %i", (long) (_newPThread->thread),
					_newPThread->PThread_count, _newPThread->isLast);
			LOG_TRACE(LOG_DEBUG, "PT: End of addPThread");
			return retcode;
		} else {
			_pthreadStruct = _pthreadStruct->nextClient;
		}
	}
	return retcode;
}

/* not really implemented yet */
void printallPThreads(struct pthread_struct *pthread_struct) {
	int count = 0;
	while (!pthread_struct->isLast) {
		//printf("PT: PThread %i = %i\n", count, (int) pthread_struct->thread);
		pthread_struct = pthread_struct->nextClient;
		count++;
	}
	printf("PT: PThread %i = %i\n", count, (int) pthread_struct->thread);

}

