/* return the content of a file as a char Pointer
 * Before reading the file content, a read lock will be made.
 */
char * readFile(struct shm_ctr_struct *shm_ctr, char *filename) {
	int retcode;
	LOG_TRACE(LOG_DEBUG, "Now in function readFile(). Filename to find: \"%s\"\t current filename: \"%s\"", filename, shm_ctr->filename);
	char * retchar = "File not found";
	/* if file found */
	if (strcmp(filename, (shm_ctr->filename)) == 0) {
		/* lock for Reading*/
		retcode = pthread_rwlock_rdlock(&(shm_ctr->rwlockFile));
		if (retcode == 0)
			LOG_TRACE(LOG_NOTICE, "Locked RWLock for Reading filename \"%s\"", shm_ctr->filename);
		char * filedata = (char*) malloc(sizeof(char) * MAX_FILE_LENGTH);
		filedata = strdup(shm_ctr->filedata);
		pthread_rwlock_unlock(&(shm_ctr->rwlockFile));
		if (retcode == 0)
			LOG_TRACE(LOG_NOTICE, "Unlocked RWLock for Reading filename \"%s\"", shm_ctr->filename);
		return filedata;

	}

	/* if last = return not found */
	else if (shm_ctr->isLast == TRUE) {
		return "File not found. No content to display.";
	}

	else {
		shm_ctr = shm_ctr->next;
		LOG_TRACE(LOG_DEBUG, "Recursive call of readFile()");
		retchar = readFile(shm_ctr, filename);
	}

	return retchar;
}