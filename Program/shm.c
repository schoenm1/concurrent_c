/*
 * File: 		shm.c
 * Author: 		Micha Schšnenberger
 * Modul:		Concurrent Programming in C
 *
 * Created:     15.04.2014
 * Project:		https://github.com/schoenm1/concurrent_c.git
 */

/* ------------------ How to user this Program ------------------
 1) ...
 2) ...
 3) ...


 ---------------------------------------------------------------*/

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <itskylib.h>

const char *REF_FILE = "./shm_sem_ref.dat";

#define PERM 0600

#define ALPHA_SIZE 256

//const int SIZE = sizeof(struct data);

int create_shm(key_t key, const char *txt, const char *etxt, int flags) {
	int shm_id = shmget(key, TOT_SHM_SIZE, flags | PERM);
	handle_error(shm_id, etxt, PROCESS_EXIT);
	return shm_id;
}

void cleanup(int shmid) {
	int retcode;
	retcode = shmctl(shmid, IPC_RMID, NULL);
	handle_error(retcode, "removing of shm failed", NO_EXIT);
}

/* round up an input integer to the next 2^x int. e.g. input = 102, output will be 128 */
int round_up_int(int input) {
	int output = FALSE;
	int until = log(TOT_SHM_SIZE) / log(2);
	printf("Now in round_up_int(). Filesize needed = %i\t until = %i\n",input, until);
	if (input > TOT_SHM_SIZE) {
			return output;
	}
	//printf("2^6 = %i\n",power(2,6));
int i;
	for (i = 2; i < until; i++) {
		if (input < power(2,i)) {
			output = power(2,i);
			printf("i = %i \t Output now set to = %i and return it.\n",i, output);
			return output;
			break;
		}

	}

	return output;
}

getpossibleshmPos() {
	//struct shm_ctr *_shm_ctr = *shm_ctr;

}

/* get next shm in linked list*/
struct shm_ctr_struct* getNext(struct shm_ctr_struct *_shm_ctr) {

	if ((_shm_ctr->next) != -1) {
		return (_shm_ctr->next);
	}

	return -1;
}

int getSizeshmPos(struct shm_ctr_struct *_shm_ctr) {
	int tmp = _shm_ctr->shm_size;
	return tmp;
}

