/* WRITE new File
 there are some conditions before writing a new file to shm
 1) file must not exist
 2) filesize can not be bigger than free space in shm
 3) search a good place in shm (SHM is a 'buddy' system') / if needed the shm will be devide
 4) make shure space is locked
 5) write file into shm
 */

/* forward declarations of functions */
char * readFile(struct shm_ctr_struct *shm_ctr, char *filename);
char * writeNewFile(struct shm_ctr_struct *shm_ctr, char *filename, char *filecontent, int filesize);
int deleteFile(struct shm_ctr_struct *shm_ctr, char *filename);
int checkifexists(struct shm_ctr_struct *shm_ctr, char *filename);
extern int round_up_int(int input);

/*
 * BEGIN OF handleFiles.c
 */

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

/* if a file should be delete, the used shared memory will be free after.
 * This function will set new pointer of the shm_ctr_struct which is handling the files */
int deleteFile(struct shm_ctr_struct *shm_ctr, char *filename) {
	LOG_TRACE(LOG_DEBUG, "In function deleteFile()");
	int retcode = FALSE;
	/* if hit, make settings */
	if (strcmp(filename, (shm_ctr->filename)) == 0) {

		/* lock the file now */
		retcode = pthread_rwlock_wrlock(&(shm_ctr->rwlockFile));
				if (retcode == 0)
					LOG_TRACE(LOG_NOTICE, "Locked RWLock for Writing new filename \"%s\"", filename);
				LOG_TRACE(LOG_NOTICE, "File \"%s\" successfully created in RWLock", place->filename);
				/*unlock the file */
				pthread_rwlock_unlock(&(shm_ctr->rwlockFile));
				if (retcode == 0)
					LOG_TRACE(LOG_NOTICE, "Unlocked RWLock for Writing filename \"%s\"", shm_ctr->filename);



				pthread_rwlock_destroy(3THR);
				LOG_TRACE(LOG_NOTICE, "RWLock now destroyed after deleting file");


		LOG_TRACE(LOG_DEBUG, "Filename \"%s\" found...", filename);

		shm_ctr->filename = "NULL";
		LOG_TRACE(LOG_DEBUG, "Filedata was until now: %s\n", shm_ctr->filedata);
		memset((shm_ctr->filedata), 0, shm_ctr->shm_size);
		shm_ctr->isfree = TRUE;
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

	/* if stringcompare = True return True */
	if (strcmp((shm_ctr->filename), filename) == 0) {
		return TRUE;
	}

	/* if at end of shm_ctr, return false */
	if (shm_ctr->isLast) {
		LOG_TRACE(LOG_DEBUG, "At the end of SHM-Block. No correct file name found.");
		return FALSE;
	}
	/* else go to next shm_ctr */
	shm_ctr = shm_ctr->next;
	retrcode = checkifexists(shm_ctr, filename);
	return retrcode;
}
