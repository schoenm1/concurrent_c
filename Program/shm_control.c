/*
 * File: 		shm_control.c
 * Author: 		Micha Schšnenberger
 * Modul:		Concurrent Programming in C
 *
 * Created:     01.05.2014
 * Project:		https://github.com/schoenm1/concurrent_c.git
 */

/* This file shm_control is a shared memory control handling which implements a 'buddy' system. */

/* forward function definition */
void print_all_shm_blocks();
void LOG_TRACE(int lvl, char *msg, ...);

/* In this file the control of shared memory is implemented */
struct shm_ctr_struct* find_shm_place(struct shm_ctr_struct *shm_ctr, int filesize) {
	struct shm_ctr_struct *ret_struct = FALSE;

	LOG_TRACE(LOG_INFORMATIONAL, "Size of shm Place is %i. IsFree = %i", shm_ctr->shm_size, shm_ctr->isfree);

	/* check if place size is bigger than filesize, but not bigger than 2 times filesize and if place is free (filename != NULL) */
	if ((shm_ctr->shm_size > filesize) && (shm_ctr->shm_size < (2 * filesize)) && (shm_ctr->isfree == TRUE)) {
		/* good place for new file */
		ret_struct = shm_ctr;
		return ret_struct;
	}

	/* check if at end of Shared Memory. If yes, return FALSE */
	if (shm_ctr == (shm_ctr->next)) {
		return ret_struct;
	}
	/* if no place were found, call recursive function with next shm_ctl */
	struct shm_ctr_struct *nextone = shm_ctr->next;
	ret_struct = find_shm_place(nextone, filesize);

	/* if a good place for the file was found, return the pointer of the address, otherwise the return is FALSE */
	return ret_struct;
}

/* Will devide the shm control into pieces. a place for a file
 is bigger than the file length, but less than 2 times the file length
 *  e.g. 32KB free block is requested
 *
 *  64KB     |       64KB    | 16KB     | ...
 *  not free |      free     |Ênot free | ...
 *
 *  the two 32KB can be combined
 *
 *  result:
 *  64KB     | 32KB | 32 KB  | 16KB     | ...
 *  not free | free | free   |Ênot free | ...
 */
int devide(struct shm_ctr_struct *shm_ctr, int untilSize) {
	int retrcode = FALSE;
	/* check if place can be devided */
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
		/* if the size is the size which should be, return TRUE */
		if (newsize == untilSize) {
			LOG_TRACE(LOG_INFORMATIONAL, "After deviding I have a good block size.");
			retrcode = TRUE;
			return retrcode;
			/* if the size is not good, call recursive the function and split it, until the size is good */
		} else {
			LOG_TRACE(LOG_NOTICE, "Recursive call in deviding because block size is to big (at moment = %i) ...", newsize);
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

/* if a file should be delete, the used shared memory will be free after.
 * This function will set new pointer of the shm_ctr_struct which is handling the files */
int deleteFile(struct shm_ctr_struct *shm_ctr, char *filename) {
	LOG_TRACE(LOG_DEBUG, "In function deleteFile()");
	int retcode = FALSE;
	/* if hit, make settings */
	if (strcmp(filename, (shm_ctr->filename)) == 0) {
		LOG_TRACE(LOG_DEBUG, "Filename found...");

		shm_ctr->filename = "NULL";
		shm_ctr->isfree = TRUE;
		printf("Filedata was: %s\n", shm_ctr->filedata);
		memset((shm_ctr->filedata), 0, shm_ctr->shm_size);
		printf("Filedata is now: %s\n", shm_ctr->filedata);
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

/* after deleting a file, the free block can perhaps be combine with a neighbouring
 *  free space.
 *  e.g.
 *  64KB     | 32 KB | 32 KB | 16KB     | ...
 *  not free | free  |Êfree  |Ênot free | ...
 *
 *  the two 32KB can be combined
 *
 *  result:
 *  64KB     |       64KB    | 16KB     | ...
 *  not free |      free     |Ênot free | ...
 */
int combine(struct shm_ctr_struct *shm_ctr) {
	LOG_TRACE(LOG_DEBUG, "In function combine()");
	int retcode = FALSE;

	/* if block is not free, leave size of block an go further */
	if (shm_ctr->isfree == FALSE) {
		LOG_TRACE(LOG_DEBUG, "Block is not free. Jump two times the blocksize\n");
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
	else if ((shm_ctr->isfree) == TRUE && (shm_ctr->next)->isfree == TRUE && (shm_ctr->shm_size) == (shm_ctr->next)->shm_size) {
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
	return retcode;
}
