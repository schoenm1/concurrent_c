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
	int retrcode = FALSE;

	printf("Size of shm Place is %i. IsFree = %i\n", shm_ctr->shm_size, shm_ctr->isfree);
	/* check if place size is bigger than filesize, but not bigger than 2 times filesize and if place is free (filename != NULL) */
	if ((shm_ctr->shm_size > filesize) && (shm_ctr->shm_size < (2 * filesize))
			&& (shm_ctr->isfree == TRUE)) {
		/* good place for new file */
		retrcode = shm_ctr;
		printf("Found a good place.\n");
		printf("Address of good shm Place is %x\n", shm_ctr);
		return retrcode;
	}

	/* check if at end of Shared Memory. If yes, return FALSE */
	struct shm_ctr *ab = shm_ctr->next;
	if (shm_ctr == ab) {
		printf(
				"At the end of all shared memory places... No hit found to enter the filename.\n");
		return retrcode;
	}

	printf(
			"Did not found a good place here. Will go to next possible place...\n");
	struct shm_ctr_struct *nextone = shm_ctr->next;
	retrcode = find_shm_place(nextone, filesize);

	return retrcode;
}
/* Will devide the shm control into pieces. a place for a file
 is bigger than the file length, but less than 2 times the file length */

int devide(struct shm_ctr_struct *shm_ctr, int untilSize) {
	printf("Now in devide(). I want to devide until I reach Block size of %i\n",
			untilSize);
	int retrcode = FALSE;
	/* check if place can be devided */
	printf("Shm_ctl ist free = %i\n",shm_ctr->isfree);

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
nextshm->filename = malloc(sizeof(char) * 128);
nextshm->filename = "NULL";


		/* check if this was last one */
		if (shm_ctr->isLast == TRUE) {
			shm_ctr->isLast = FALSE;
			nextshm->isLast = TRUE;
			nextshm->next = nextshm;
		}

		if (newsize == untilSize) {
			printf("After deviding I have a good block size.\n");
			retrcode = TRUE;
			return retrcode;
		} else {
			//print_single_shm_blocks(shm_ctr);
			//print_single_shm_blocks(shm_ctr->next);
			printf(
					"Recursive call in deviding because block size is to big (at moment = %i) ... \n",
					newsize);
			retrcode = devide(shm_ctr, untilSize);
		}

	}

	/* if block is not free */
	else{
		shm_ctr = (shm_ctr->next)->next;
		retrcode = devide(shm_ctr, untilSize);
	}

	return retrcode;
}

int check_combine() {

	return -1;
}

int combine() {

	return -1;
}

