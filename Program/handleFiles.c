

/* WRITE new File
 there are some conditions before writing a new file to shm
 1) file must not exist
 2) filesize can not be bigger than free space in shm
 3) search a good place in shm (SHM is a 'buddy' sytem') / if need devide shm
 4) make shure space is locked
 5) write file into shm
 */

int writeNewFile(char *filename, int filesize){
int totfilesize = filesize + 1; //because termination \0

if (checkifexists()){
	return -1; //this means file already exist, can not create it
}










return -1;
}





/* not implemented yet */
int checkifexists(){



	return -1;
}

