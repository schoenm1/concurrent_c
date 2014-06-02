/*
 * File: 		pthread_control.c
 * Author: 		Micha Schšnenberger
 * Modul:		Concurrent Programming in C
 *
 * Created:     26.05.2014
 * Project:		https://github.com/schoenm1/concurrent_c.git
 */

/* forward declarations of functions */
int joiningAllPThreads(struct pthread_struct *myPThreadList);
int addPThread(struct pthread_struct *_pthreadStruct, pthread_t pThread, int counter);

/*
 * BEGIN OF pthread_control.c
 */

/* join all PThreads - if PThread is not ended, this function blocks the MainThread of the Server
 * This function should just be called, if the server wants to shutdown
 */
int joiningAllPThreads(struct pthread_struct *myPThreadList) {
	unsigned long pThread_ID;
	struct pthread_struct *_myPThread = myPThreadList;
	struct pthread_struct *_nextPThread = _myPThread->nextClient;
	int retcode = FALSE;
	LOG_TRACE(LOG_INFORMATIONAL, "PT: In Joining PThreads()");

	/* joining all PThread except the last one */
	while (!_myPThread->isLast) {
		pThread_ID = _myPThread->thread;
		pthread_join(_myPThread->thread, NULL);
		LOG_TRACE(LOG_INFORMATIONAL, "PT: Successfully joined PThread %lu", pThread_ID);
		_myPThread = _nextPThread;
		_nextPThread = _nextPThread->nextClient;
	}

	/* joining the last PThread*/
	retcode = pthread_join(_myPThread->thread, NULL);
	LOG_TRACE(LOG_INFORMATIONAL, "PT: Successfully joined PThread %lu", pThread_ID);
	handle_error(retcode, "PT: Failing to join PThread", NO_EXIT);
	return retcode;
}

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

