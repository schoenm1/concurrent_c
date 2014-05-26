/* WRITE new File
 there are some conditions before writing a new file to shm
 1) file must not exist
 2) filesize can not be bigger than free space in shm
 3) search a good place in shm (SHM is a 'buddy' sytem') / if need devide shm
 4) make shure space is locked
 5) write file into shm
 */

/** init the mutex arary in for the file */
void initMutexesperFile(struct shm_ctr_struct *shm_ctr) {
	int count = 10;
	LOG_TRACE(LOG_INFORMATIONAL, "Size of Mutex Array = %i", count);
	for (int i = 0; i < count; ++i) {
		pthread_mutex_init(&(shm_ctr->filemutex[i]), NULL);
	}
}


/* search in Array for one mutex which is not locked */
int lockOneMutexForReading(struct shm_ctr_struct *shm_ctr) {
	int retcode = FALSE;
	for (int i = 0; i < 10; ++i) {

		retcode = pthread_mutex_lock(&(shm_ctr->filemutex[i]));
		printf("# Retcode = %i", retcode);
		if (retcode == TRUE)
			return TRUE;
	}

	return FALSE;
}

char * readFile(struct shm_ctr_struct *shm_ctr, char *filename) {
	lockOneMutexForReading(shm_ctr);
	LOG_TRACE(LOG_DEBUG, "Now in funtion readFile()\n");
	char * retchar = "File not found";
	/* if file found */
	if (strcmp(filename, (shm_ctr->filename)) == 0) {
		return shm_ctr->filedata;
	}

	/* if last = return not found */
	else if (shm_ctr->isLast == TRUE) {
		return "File not found. No content to display.";
	}

	else {
		shm_ctr = (shm_ctr->next)->next;
		LOG_TRACE(LOG_DEBUG, "Recursive call of readFile()\n");
		retchar = readFile(shm_ctr, filename);
	}

	return retchar;
}

char * writeNewFile(struct shm_ctr_struct *shm_ctr, char *filename, char *filecontent, int filesize) {
	printf("Now in Function writeNewFile()\n");
	int retcode;
	//int totfilesize = filesize + 1; //because termination \0
	char *returnchar = malloc(sizeof(char) * 64);
	memset(returnchar, '\0', sizeof(returnchar)); //clear return String

	/* check if file already exists */
	retcode = checkifexists(shm_ctr, filename);
	printf("File exists = %i\n", retcode);

	if (retcode == TRUE) {
		printf("File already exists\n");
		return "File already exists\n";
	}
	/* if file does not exists, create a new file */
	printf("File with name %s does not exists. I will create it.\n", filename);

	printf("Address of shm Place to check is %p\n", shm_ctr);
	struct shm_ctr_struct *place = find_shm_place(shm_ctr, filesize);
	printf("Checked a good address is:  %p\n", place);

	if (place == FALSE) {
		printf("0 is not valid. So there is no good place to write the file into... Trying no to devide the Shared Memory...\n");
		int block_size_needed = round_up_int(filesize);
		retcode = devide(shm_ctr, block_size_needed);
		if (!retcode) {
			return "There was no place in the shared memory for creating the file!\n";
		}
		place = find_shm_place(shm_ctr, filesize);
	}

	if (place != FALSE) {
		place->isfree = FALSE;
		place->filename = strdup(filename);
		place->filedata = strdup(filecontent);
		initMutexesperFile(place);
		return getSingleString("File \"%s\" successfully created.\n", (place->filename));
	}
	return -1;
}

/* will check if the file currenctly exists in shm */
int checkifexists(struct shm_ctr_struct *shm_ctr, char *filename) {
	int retrcode = FALSE;
	/*printf("Searching for Filename = %s\n", filename);

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
