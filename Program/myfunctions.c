/*  power recursive  return x^y */
int power(int x, int y) {
	if (y == 0)
		return 1;
	if (y == 1)
		return x;
	return (x * power(x, y - 1));
}

/* will output all existing shared memory blocks which exists at the moment */
void print_all_shm_blocks(struct shm_ctr_struct *shm_ctr) {
	struct shm_ctr_struct *myshm_ctr = shm_ctr;
	int i = 1;

	printf("\nWill now output all shm-blocks...\n");
	printf("===============================================================================\n");
	while (TRUE) {
		printf("Block No %i:\t Block-Address = %p\t\t Block-Size = %i\t isFree = %i Filename = %s\t PTR Filename = %p\t isLast = %i\n", i,
				myshm_ctr, myshm_ctr->shm_size, myshm_ctr->isfree, myshm_ctr->filename, &(myshm_ctr->filename), myshm_ctr->isLast);
		if (myshm_ctr->isLast == TRUE) {
			break;
		} else {
			myshm_ctr = myshm_ctr->next;
			i++;
		}
	}
	printf("===============================================================================\n");

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

char * get_all_shm_blocks(struct shm_ctr_struct *shm_ctr) {
	struct shm_ctr_struct *myshm_ctr = shm_ctr;
	char * one = "=============================================== ALL BLOCKS OF SHARED MEMORY ===============================================\n";
	char * all_shm_blocks = (char *) malloc(8192);
	strcpy(all_shm_blocks, one);

	int i = 1;

	while (TRUE) {
		char * myblock = (char *) malloc(sizeof(malloc) * 256);
		myblock = getSingleString(
				"Block No %i:\t Block-Address = %x\t\t Block-Size = %i\t isFree = %i Filename = %s\t PTR Filename = %x\t isLast = %i\n", i,
				myshm_ctr, myshm_ctr->shm_size, myshm_ctr->isfree, myshm_ctr->filename, &(myshm_ctr->filename), myshm_ctr->isLast);
		strcat(all_shm_blocks, myblock);

		free(myblock);
		if (myshm_ctr->isLast == TRUE) {
			break;
		} else {
			myshm_ctr = myshm_ctr->next;
			i++;
		}
	}
	char * last = "=============================================== END OF SHARED MEMORY ===============================================\n";
	strcat(all_shm_blocks, last);

	return all_shm_blocks;
}

void print_single_shm_blocks(struct shm_ctr_struct *shm_ctr) {
	printf("=====================================================================================================\n");
	printf("Address: %p\t Block-Size = %i\t Filename = %s\n", shm_ctr->filedata, shm_ctr->shm_size, shm_ctr->filename);

	printf("=====================================================================================================\n");
}

