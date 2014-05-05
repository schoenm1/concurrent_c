/*  power recursive  return x^y */
int power(int x, int y) {
	if (y == 0)
		return 1;
	if (y == 1)
		return x;
	return (x * power(x, y - 1));
}



void copystr(char *dest,char *src)
       {
                while(*src!='\0')
                *dest++=*src++;
                *dest='\0';
                return;
       }





/* will output all existing shared memory blocks which exists at the moment */
void print_all_shm_blocks(struct shm_ctr_struct *shm_ctr) {
	struct shm_ctr_struct *myshm_ctr = shm_ctr;
	int i = 1;
	printf("\nWill now output all shm-blocks...\n");
	printf(
			"===============================================================================\n");
	while (TRUE) {
		printf("Block No %i:\t Block-Size = %i\t isFree = %i Filename = %s\t isLast = %i\n",
				i, myshm_ctr->shm_size, myshm_ctr->isfree, myshm_ctr->filename,  myshm_ctr->isLast);
		if (myshm_ctr->isLast == TRUE) {
			break;
		} else {
			myshm_ctr = myshm_ctr->next;
			i++;
		}
	}
	printf(
			"===============================================================================\n");

}

void print_single_shm_blocks(struct shm_ctr_struct *shm_ctr) {
	printf(
			"===============================================================================\n");
	printf("Address: %x\t Block-Size = %i\t Filename = %s\n", shm_ctr->filedata,
			shm_ctr->shm_size, shm_ctr->filename);

	printf(
			"===============================================================================\n");
}

