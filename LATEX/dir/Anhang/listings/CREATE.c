/* write a new file with its content to the shared memory.
 * @return: Pointer to the shm control struct where the pointer of the filename and filecontent is
 */
char * writeNewFile(struct shm_ctr_struct *shm_ctr, char *filename, char *filecontent, int filesize) {
	LOG_TRACE(LOG_DEBUG, "Now in Function writeNewFile()");
	int retcode;
	char *returnchar = malloc(sizeof(char) * 64);
	memset(returnchar, '\0', sizeof(returnchar)); //clear return String

	/* check if file already exists */
	retcode = checkifexists(shm_ctr, filename);
	/* if extist, return "File already exist */
	if (retcode == TRUE) {
		LOG_TRACE(LOG_INFORMATIONAL, "File \"%s\" already exists", filename);
		return "File already exist\n";
	}
	/* if file does not exists, create a new file */
	LOG_TRACE(LOG_INFORMATIONAL, "File with name %s does not exists. I will create it.", filename);

	LOG_TRACE(LOG_DEBUG, "Address of shm Place to check is %p", shm_ctr);
	struct shm_ctr_struct *place = find_shm_place(shm_ctr, filesize);
	LOG_TRACE(LOG_DEBUG, "Checked a good address is:  %p", place);

	if (place == FALSE) {
		LOG_TRACE(LOG_DEBUG,
				"0 is not valid. So there is no good place to write the file into... Trying no to devide the Shared Memory...");

		int block_size_needed = round_up_int(filesize);
		retcode = devide(shm_ctr, block_size_needed);
		if (!retcode) {
			return "There was no place in the shared memory for creating the file!";
		}
		place = find_shm_place(shm_ctr, filesize);
	}

	/* if a good place was found */
	if (place != FALSE) {
		place->isfree = FALSE;
		/* init the read write lock for this file */
		pthread_rwlock_init(&(place->rwlockFile), NULL);/* Default initialization */
		/* lock the file */
		retcode = pthread_rwlock_wrlock(&(shm_ctr->rwlockFile));
		if (retcode == 0)
			LOG_TRACE(LOG_NOTICE, "Locked RWLock for Writing new filename \"%s\"", filename);
		place->filename = strdup(filename);
		place->filedata = strdup(filecontent);
		LOG_TRACE(LOG_NOTICE, "File \"%s\" successfully created in RWLock", place->filename);
		/*unlock the file */
		pthread_rwlock_unlock(&(shm_ctr->rwlockFile));
		if (retcode == 0)
			LOG_TRACE(LOG_NOTICE, "Unlocked RWLock for Writing filename \"%s\"", shm_ctr->filename);

		return getSingleString("File \"%s\" successfully created.", (place->filename));
	}
	return -1;
}