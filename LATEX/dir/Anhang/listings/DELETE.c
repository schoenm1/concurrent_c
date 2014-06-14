/* if a file should be delete, the used shared memory will be free after.
 * This function will set new pointer of the shm_ctr_struct which is handling the files */
int deleteFile(struct shm_ctr_struct *shm_ctr, char *filename) {
	LOG_TRACE(LOG_DEBUG, "In function deleteFile()");
	int retcode = FALSE;
	/* if hit, make settings */
	if (strcmp(filename, (shm_ctr->filename)) == 0) {
		LOG_TRACE(LOG_DEBUG, "Filename \"%s\" found...", filename);

		/* lock the file now */
		retcode = pthread_rwlock_wrlock(&(shm_ctr->rwlockFile));
		if (retcode == 0)
			LOG_TRACE(LOG_NOTICE, "Locked RWLock for deleting filename \"%s\"", filename);

		/* deleting file content, filename.... */
		shm_ctr->filename = "NULL";
		LOG_TRACE(LOG_DEBUG, "Filedata was until now: %s\n", shm_ctr->filedata);
		memset((shm_ctr->filedata), 0, shm_ctr->shm_size);
		shm_ctr->isfree = TRUE;

		/*unlock the file */
		pthread_rwlock_unlock(&(shm_ctr->rwlockFile));
		if (retcode == 0)
			LOG_TRACE(LOG_NOTICE, "Unlocked RWLock for deleting filename \"%s\"", shm_ctr->filename);

		pthread_rwlock_destroy(&(shm_ctr->rwlockFile));
		LOG_TRACE(LOG_NOTICE, "RWLock now destroyed after deleting file");

		return TRUE;
	}
	/* if at end of SHM and no hit, return FALSE */
	else if (shm_ctr->isLast == TRUE) {
		LOG_TRACE(LOG_DEBUG, "At the end of SHM. No filename found.");
		return FALSE;
	}
	/* if no hit and not at end of shm, go to next */
	else {
		LOG_TRACE(LOG_DEBUG, "Recursive call of function deleteFile()");
		retcode = deleteFile((shm_ctr->next), filename);
	}
	return retcode;
}