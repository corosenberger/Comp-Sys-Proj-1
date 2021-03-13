#include "solver.h"
#include "utils.h"

#define SEM_ID    250	 /* ID for the semaphore.               */

struct solution solver_oneprocess(int len, int* data){
	struct solution sol;
	int sum = 0;
	sol.max = 0;
	sol.num_hidden = 0;

	for(int i = 0; i < len; i++){
		if (data[i] > 0){
			if (data[i] > sol.max){
				sol.max = data[i];
			}
			sum += data[i];
		}
		else{
			sol.num_hidden += 1;
			printf("Hi I'm process 0 and I found the hidden key in position %d\n", i);
		}
	}
	sol.avg = (float)sum / (float)len;

	return sol;
}

struct solution solver_DFS(int len, int* data, int NP){
	// 1. create a shared memory space (3 ints possibly).
	// 2. spawn one child recursively from each child until num_children reaches NP.
	// 3. work some and wait for your child, and then exit. 
	// 4. only the parent remains at the end. Compile the results and exit.
	// A part of the code was inspired from Source: http://www.cs.kent.edu/~ruttan/sysprog/lectures/shmem/shared-mem-with-semaphore.c
	int sem_set_id;            /* ID of the semaphore set.           */
    union semun {              /* semaphore value, for semctl().     */
                int val;
                struct semid_ds *buf;
                ushort * array;
        } sem_val;    
    int shm_id;	      	       /* ID of the shared memory segment.   */
    char* shm_addr; 	       /* address of shared memory segment.  */
    struct solution* sol;
    int* num_processes;
    struct shmid_ds shm_desc;
    int rc;		       /* return value of system calls.      */
    pid_t pid;		       /* PID of child process.              */
    int child_id = 0;

    /* create a semaphore set with ID 250, with one semaphore   */
    /* in it, with access only to the owner.                    */
    sem_set_id = semget(SEM_ID, 1, IPC_CREAT | 0600);
    if (sem_set_id == -1) {
		perror("main: semget");
		exit(1);
    }

    /* intialize the first (and single) semaphore in our set to '1'. */
    sem_val.val = 1;
    rc = semctl(sem_set_id, 0, SETVAL, sem_val);
    if (rc == -1) {
	perror("main: semctl");
	exit(1);
    }

    /* allocate a shared memory segment with size of 2048 bytes. */
    shm_id = shmget(100, 2048, IPC_CREAT | IPC_EXCL | 0600);
    if (shm_id == -1) {
        perror("main: shmget: ");
        exit(1);
    }

    /* attach the shared memory segment to our process's address space. */
    shm_addr = shmat(shm_id, NULL, 0);
    if (!shm_addr) { /* operation failed. */
        perror("main: shmat: ");
        exit(1);
    }

    /* allocate the struct pointer to the shared memory */
    num_processes = (int*) shm_addr;
    *num_processes = 0;
    sol = (struct solution*) ((void*)shm_addr+sizeof(int));

    /* fork-off a child process that'll populate the memory segment. */
    pid = fork();
    switch (pid) {
	case -1:
	    perror("fork: ");
	    exit(1);
	    break;
	case 0:
	    do_child_DFS(len, data, NP, child_id, sem_set_id, num_processes, sol);
	    exit(0);
	    break;
	default:
	    do_parent_DFS(len, data, NP, sem_set_id, num_processes, sol);
	    break;
    }

    /* wait for child process's terination. */
    {
        int child_status;

        wait(&child_status);
    }

    // copy the answers and post-process
    struct solution mysol;
    mysol.max = sol->max;
    mysol.avg = sol->avg / (float)len;
    mysol.num_hidden = sol->num_hidden;

    /* detach the shared memory segment from our process's address space. */
    if (shmdt(shm_addr) == -1) {
        perror("main: shmdt: ");
    }

    /* de-allocate the shared memory segment. */
    if (shmctl(shm_id, IPC_RMID, &shm_desc) == -1) {
        perror("main: shmctl: ");
    }

	return mysol;
}

void do_child_DFS(int len, int* data, int NP, int child_id, int sem_set_id, int* num_processes, struct solution* sol){
	// TODO: check if want to spawn more children. 

	// do some work
	int max = 0;
	int sum = 0;
	int num_hidden = 0;
	int parts = len/NP;
	for(int i = child_id*parts; i < (child_id+1)*parts; i++){
		if (data[i] > 0){
			if (data[i] > max){
				max = data[i];
			}
			sum += data[i];
		}
		else{
			num_hidden += 1;
			printf("Hi I'm process %d and I found the hidden key in position %d\n", child_id+1, i);
		}
	}
	sem_lock(sem_set_id);
	if(sol->max < max) sol->max = max;
	sol->avg += (float) sum;
	sol->num_hidden += num_hidden;
	sem_unlock(sem_set_id);
}

void do_parent_DFS(int len, int* data, int NP, int sem_set_id, int* num_processes, struct solution* sol){
	// parent always processes the last segment!
	int max = 0;
	int sum = 0;
	int num_hidden = 0;
	int parts = len/NP;
	for(int i = (NP-1)*parts; i < len; i++){
		if (data[i] > 0){
			if (data[i] > max){
				max = data[i];
			}
			sum += data[i];
		}
		else{
			num_hidden += 1;
			printf("Hi I'm process %d and I found the hidden key in position %d\n", 0, i);
		}
	}
	sem_lock(sem_set_id);
	if(sol->max < max) sol->max = max;
	sol->avg += (float) sum;
	sol->num_hidden += num_hidden;
	sem_unlock(sem_set_id);
}