/* WRITE new File
 there are some conditions before writing a new file to shm
 1) file must not exist
 2) filesize can not be bigger than free space in shm
 3) search a good place in shm (SHM is a 'buddy' sytem') / if need devide shm
 4) make shure space is locked
 5) write file into shm
 */

/* forward function declaration */
int checkifexists(struct shm_ctr_struct *shm_ctr, char *filename);

char * readFile(struct shm_ctr_struct *shm_ctr, char *filename) {
	int retcode;
	LOG_TRACE(LOG_DEBUG, "Now in function readFile()");
	char * retchar = "File not found";
	/* if file found */
	if (strcmp(filename, (shm_ctr->filename)) == 0) {
		/* lock for Reading*/
		retcode = pthread_rwlock_rdlock(&(shm_ctr->rwlockFile));
		if (retcode == 0)
			LOG_TRACE(LOG_INFORMATIONAL, "Locked RWLock for Reading filename \"%s\"", shm_ctr->filename);
		char * filedata = (char*) malloc(sizeof(char) * MAX_FILE_LENGTH);
		filedata = strdup(shm_ctr->filedata);
		pthread_rwlock_unlock(&(shm_ctr->rwlockFile));
		if (retcode == 0)
			LOG_TRACE(LOG_INFORMATIONAL, "Unlocked RWLock for Reading filename \"%s\"", shm_ctr->filename);
		return filedata;

	}

	/* if last = return not found */
	else if (shm_ctr->isLast == TRUE) {
		return "File not found. No content to display.";
	}

	else {
		shm_ctr = (shm_ctr->next)->next;
		LOG_TRACE(LOG_DEBUG, "Recursive call of readFile()");
		retchar = readFile(shm_ctr, filename);
	}

	return retchar;
}

char * writeNewFile(struct shm_ctr_struct *shm_ctr, char *filename, char *filecontent, int filesize) {
	LOG_TRACE(LOG_DEBUG, "Now in Function writeNewFile()");
	int retcode;
	//int totfilesize = filesize + 1; //because termination \0
	char *returnchar = malloc(sizeof(char) * 64);
	memset(returnchar, '\0', sizeof(returnchar)); //clear return String

	/* check if file already exists */
	retcode = checkifexists(shm_ctr, filename);

	if (retcode == TRUE) {
		LOG_TRACE(LOG_DEBUG, "File \"%s\" already exists", filename);
		return "File already exists\n";
	}
	/* if file does not exists, create a new file */
	LOG_TRACE(LOG_DEBUG, "File with name %s does not exists. I will create it.", filename);

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

	if (place != FALSE) {
		place->isfree = FALSE;
		place->filename = strdup(filename);
		place->filedata = strdup(filecontent);

		/* init the read write lock for this file */
		pthread_rwlock_init(&(place->rwlockFile), NULL);/* Default initialization */

		return getSingleString("File \"%s\" successfully created.", (place->filename));
	}
	return -1;
}

/* if a file should be delete, the used shared memory will be free after.
 * This function will set new pointer of the shm_ctr_struct which is handling the files */
int deleteFile(struct shm_ctr_struct *shm_ctr, char *filename) {
	LOG_TRACE(LOG_DEBUG, "In function deleteFile()");
	int retcode = FALSE;
	/* if hit, make settings */
	if (strcmp(filename, (shm_ctr->filename)) == 0) {
		LOG_TRACE(LOG_DEBUG, "Filename \"%s\" found...", filename);

		shm_ctr->filename = "NULL";
		shm_ctr->isfree = TRUE;
		printf("Filedata was: %s\n", shm_ctr->filedata);
		memset((shm_ctr->filedata), 0, shm_ctr->shm_size);
		//printf("Filedata is now: %s\n", shm_ctr->filedata);
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

/* will check if the file currenctly exists in shm */
int checkifexists(struct shm_ctr_struct *shm_ctr, char *filename) {
	int retrcode = FALSE;
	//printf("Searching for Filename = %s\n", filename);

	/* if stringcompare = True return True */
	if (strcmp((shm_ctr->filename), filename) == 0) {
		return TRUE;
	}

	/* if at end of shm_ctr, return false */
	if (shm_ctr->isLast) {
		printf("At the end of SHM-Block. No correct file name found.\n");
		return FALSE;
	}
	/* else go to next shm_ctr */
	shm_ctr = shm_ctr->next;
	retrcode = checkifexists(shm_ctr, filename);
	return retrcode;
}
