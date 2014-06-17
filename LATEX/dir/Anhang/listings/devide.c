/* Will devide the shm control into pieces. a place for a file
 is bigger than the file length, but less than 2 times the file length
 *  e.g. 32KB free block is requested
 *
 *  64KB     |       64KB    | 16KB     | ...
 *  not free |      free     | not free | ...
 *
 *  the two 32KB can be combined
 *
 *  result:
 *  64KB     | 32KB | 32 KB  | 16KB     | ...
 *  not free | free | free   | not free | ...
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
			LOG_TRACE(LOG_INFORMATIONAL, "PT: After deviding I have a good block size.");
			retrcode = TRUE;
			return retrcode;
			/* if the size is not good, call recursive the function and split it, until the size is good */
		} else {
			if (newsize <= 2)
				return FALSE;

			LOG_TRACE(LOG_NOTICE, "PT: Recursive call in deviding because block size is to big (at moment = %i / should be: %i) ...",
					newsize, untilSize);
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