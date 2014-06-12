/*
 * File:   		myfunctions.c
 * Author: 		Micha Schšnenberger
 * Modul:		Concurrent Programming in C
 *
 * Created:     09.04.2014
 * Project:		https://github.com/schoenm1/concurrent_c.git
 */

/* This file contains self-written functions, which are needed for the Server.c
 *
 */

/*  power recursive  return x^y */
int power(int x, int y) {
	if (y == 0)
		return 1;
	if (y == 1)
		return x;
	return (x * power(x, y - 1));
}

/* returns a char pointer with a fix length. This is needed for a synchronized output */
char * getfixCharLen(char *mychar, int mylength) {
	char * retchar = strdup(mychar);
	while (strlen(retchar) < mylength) {
		strcat(retchar, " ");
	}
	return retchar;
}

/* round up an input integer to the next 2^x int. e.g. input = 102, output will be 128 */
int round_up_int(int input) {
	int output = FALSE;
	int until = log(TOT_SHM_SIZE) / log(2);
	if (input > TOT_SHM_SIZE) {
		return output;
	}
	int i;
	for (i = 2; i < until; i++) {
		if (input < power(2, i)) {
			output = power(2, i);
			LOG_TRACE(LOG_INFORMATIONAL, "i = %i \t Output now set to = %i and return it.", i, output);
			return output;
			break;
		}
	}
	return output;
}

/* make from multiple arguments one string */
char * getSingleString(char *msg, ...) {
	char buf[1024];
	char *buff = (char *) malloc(sizeof(char) * 65536);
	char *retMsg = (char *) malloc(sizeof(char) * 65536);

	va_list va;
	va_start(va, msg);
	vsprintf(buf, msg, va);
	sprintf(buff, "%s", buf);
	retMsg = strdup(buff);
	free(buff);
	va_end(va);
	return retMsg;
}

/* prepares the output of all shared memory blocks to send to the client over TCP/IP */
char * get_all_shm_blocks(struct shm_ctr_struct *shm_ctr) {
	struct shm_ctr_struct *myshm_ctr = shm_ctr;

	char * one =
			"=============================================== ALL BLOCKS OF SHARED MEMORY ========================================================================\n";
	char * all_shm_blocks = (char *) malloc(8192);
	strcpy(all_shm_blocks, one);

	int i = 1;

		while (TRUE) {
			char * myblock = (char *) malloc(sizeof(malloc) * 256);
			myblock = getSingleString(
					"Block No %i:\t Block-Address = %x\t\t Block-Size = %i\t isFree = %i Filename = %s\t PTR Filename = %x   \t isLast = %i\n",
					i, myshm_ctr, myshm_ctr->shm_size, myshm_ctr->isfree, getfixCharLen(myshm_ctr->filename, 20), &(myshm_ctr->filename),
					myshm_ctr->isLast);
			strcat(all_shm_blocks, myblock);

			free(myblock);
			if (myshm_ctr->isLast == TRUE) {
				break;
			} else {
				myshm_ctr = myshm_ctr->next;
				i++;
			}
		}
	char * last =
			"============================================================== END OF SHARED MEMORY =======================================================================================\n";
	strcat(all_shm_blocks, last);

	return all_shm_blocks;
}

/* print single block of shared memory. Just used for manual debugging */
void print_single_shm_blocks(struct shm_ctr_struct *shm_ctr) {
	printf("=====================================================================================================\n");
	printf("Block-Address = %x\t\t Block-Size = %i\t isFree = %i Filename = %s\t PTR Filename = %x   \t isLast = %i\n", shm_ctr,
			shm_ctr->shm_size, shm_ctr->isfree, getfixCharLen(shm_ctr->filename, 20), &(shm_ctr->filename), shm_ctr->isLast);

	printf("=====================================================================================================\n");
}

