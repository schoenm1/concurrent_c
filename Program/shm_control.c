/*
 * File: 		shm_control.c
 * Author: 		Micha Schšnenberger
 * Modul:		Concurrent Programming in C
 *
 * Created:     01.05.2014
 * Project:		https://github.com/schoenm1/concurrent_c.git
 */

/* In this file the control of shared memory is implemented */

struct shm_ctr_struct* find_shm_place(struct shm_ctr_struct *shm_ctr,
		int filesize) {

	struct shm_ctr_struct *ret_struct = FALSE;

	LOG_TRACE(LOG_INFORMATIONAL, "Size of shm Place is %i. IsFree = %i\n",
			shm_ctr->shm_size, shm_ctr->isfree);
	/* check if place size is bigger than filesize, but not bigger than 2 times filesize and if place is free (filename != NULL) */
	if ((shm_ctr->shm_size > filesize) && (shm_ctr->shm_size < (2 * filesize))
			&& (shm_ctr->isfree == TRUE)) {
		/* good place for new file */
		ret_struct = shm_ctr;
		//	printf("Found a good place.\n");
		//	printf("Address of good shm Place is %p\n", shm_ctr);
		//	printf("Address of good shm Place is %i\n", &shm_ctr);
		return ret_struct;
	}

	/* check if at end of Shared Memory. If yes, return FALSE */
	//struct shm_ctr *ab = shm_ctr->next;
	if (shm_ctr == (shm_ctr->next)) {
		//printf("At the end of all shared memory places... No hit found to enter the filename.\n");
		return ret_struct;
	}

	//printf("Did not found a good place here. Will go to next possible place...\n");
	struct shm_ctr_struct *nextone = shm_ctr->next;
	ret_struct = find_shm_place(nextone, filesize);

	return ret_struct;
}
/* Will devide the shm control into pieces. a place for a file
 is bigger than the file length, but less than 2 times the file length */

int devide(struct shm_ctr_struct *shm_ctr, int untilSize) {
	//printf("Now in devide(). I want to devide until I reach Block size of %i\n",untilSize);
	int retrcode = FALSE;
	/* check if place can be devided */
//	printf("Shm_ctl ist free = %i\n",shm_ctr->isfree);
	if (shm_ctr->isfree == TRUE) {

		/* new setting for devided, next place */
		struct shm_ctr_struct *nextshm;
		nextshm = malloc(sizeof(struct shm_ctr_struct));

		struct shm_ctr_struct *tmpnext = shm_ctr->next;

		/* new setting for this place */
		int newsize = (shm_ctr->shm_size) / 2;
		shm_ctr->shm_size = newsize; // setting size
		shm_ctr->next = nextshm; //setting next

		/* new setting for next place */
		nextshm->prev = shm_ctr; // setting prev = this
		nextshm->shm_size = newsize; // setting size
		nextshm->isfree = TRUE; //setting isFree
		nextshm->next = tmpnext; //setting next
		nextshm->isLast = FALSE; // set is Last = 0
		nextshm->filename = malloc(sizeof(char) * 128);
		nextshm->filename = "NULL";
		nextshm->filedata = (shm_ctr->filedata) + (shm_ctr->shm_size);

		/* check if this was last one */
		if (shm_ctr->isLast == TRUE) {
			shm_ctr->isLast = 0; //FALSE
			nextshm->isLast = 1; //TRUE
			nextshm->next = nextshm;
		}

		if (newsize == untilSize) {
			LOG_TRACE(LOG_INFORMATIONAL,
					"After deviding I have a good block size.\n");

			retrcode = TRUE;
			return retrcode;
		} else {
			//print_single_shm_blocks(shm_ctr);
			//print_single_shm_blocks(shm_ctr->next);

			LOG_TRACE(LOG_NOTICE,
					"Recursive call in deviding because block size is to big (at moment = %i) ... \n",
					newsize);

			retrcode = devide(shm_ctr, untilSize);
		}

	}

	/* if block is not free */
	else {
		shm_ctr = (shm_ctr->next)->next;
		retrcode = devide(shm_ctr, untilSize);
	}

	return retrcode;
}

int deleteFile(struct shm_ctr_struct *shm_ctr, char *filename) {
	LOG_TRACE(LOG_DEBUG, "In function deleteFile()\n");
	int retcode = FALSE;
//	LOG_TRACE(LOG_DEBUG, "In function deleteFile()\n");
	/* if hit, make settings */
	if (strcmp(filename, (shm_ctr->filename)) == 0) {
		LOG_TRACE(LOG_DEBUG, "Filename found...\n");

		shm_ctr->filename = "NULL";
		shm_ctr->isfree = TRUE;
		printf("Filedata was: %s\n", shm_ctr->filedata);
		memset((shm_ctr->filedata), 0, shm_ctr->shm_size);
		printf("Filedata is now: %s\n", shm_ctr->filedata);
		return TRUE;
	}

	/* if at end of SHM and no hit, return FALSE */
	else if (shm_ctr->isLast == TRUE) {
		LOG_TRACE(LOG_DEBUG, "At the end of SHM. No filename found.\n");
		return FALSE;
	}

	/* if no hit and not at end of shm, go to next */
	else {
		LOG_TRACE(LOG_DEBUG, "Recursive call of function deleteFile()\n");
		retcode = deleteFile((shm_ctr->next), filename);
	}

	return retcode;
}

int combine(struct shm_ctr_struct *shm_ctr) {
	LOG_TRACE(LOG_DEBUG, "In function combine()\n");
	int retcode = FALSE;

	/* if block is not free, leave size of block an go further */
	if (shm_ctr->isfree == FALSE) {
		LOG_TRACE(LOG_DEBUG,
				"Block is not free. Jump two times the blocksize\n");
		//print_all_shm_blocks(shm_ctr);
		retcode = combine((shm_ctr->next)->next);

	}

	/* if at end of shm return FALSE */
	else if (shm_ctr->isLast == TRUE) {
		LOG_TRACE(LOG_DEBUG, "At end of SHM. Return FALSE\n");
		print_all_shm_blocks(shm_ctr);
		return retcode;
	}

	/* if shm block is free and next is free */
	else if ((shm_ctr->isfree) == TRUE && (shm_ctr->next)->isfree == TRUE
			&& (shm_ctr->shm_size) == (shm_ctr->next)->shm_size) {
		LOG_TRACE(LOG_DEBUG, "Found two blocks with same size and both free\n");
		struct shm_ctr_struct *tmpnext = shm_ctr->next;

		shm_ctr->shm_size = 2 * (shm_ctr->shm_size);

		/* if next was last, point next now to myself */
		if ((shm_ctr->next)->isLast == TRUE) {
			shm_ctr->isLast = TRUE; //FALSE
			shm_ctr->next = shm_ctr;
		} else {
			shm_ctr->next = tmpnext->next;
		}

		retcode = TRUE;
	}

	/* otherwhise, go to next block */
	else {
		LOG_TRACE(LOG_DEBUG, "Nothing to to. Go to next block.\n");
		retcode = combine(shm_ctr->next);

	}

//free(tmpnext);

	return retcode;
}
int check_combine() {

	return -1;
}

