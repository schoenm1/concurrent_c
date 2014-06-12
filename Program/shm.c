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

/* forward declarations of functions */

int create_shm(key_t key, const char *txt, const char *etxt, int flags);
void cleanup(int shmid);
struct shm_ctr_struct* getNext(struct shm_ctr_struct *_shm_ctr);
int getSizeshmPos(struct shm_ctr_struct *_shm_ctr);

/*
 * BEGIN OF shm.c
 */

/* creates the shared memory */
int create_shm(key_t key, const char *txt, const char *etxt, int flags) {
	int shm_id = shmget(key, TOT_SHM_SIZE, flags | PERM);
	handle_error(shm_id, etxt, PROCESS_EXIT);
	return shm_id;
}

/* clean up the shared memory */
void cleanup(int shmid) {
	int retcode;
	retcode = shmctl(shmid, IPC_RMID, NULL);
	handle_error(retcode, "Removing of shm failed", NO_EXIT);
}

/* get next shm in linked list*/
struct shm_ctr_struct* getNext(struct shm_ctr_struct *_shm_ctr) {
	if ((_shm_ctr->next) != -1) {
		return (_shm_ctr->next);
	}
	return -1;
}
/* returns the size of the shm-block */
int getSizeshmPos(struct shm_ctr_struct *_shm_ctr) {
	int tmp = _shm_ctr->shm_size;
	return tmp;
}

