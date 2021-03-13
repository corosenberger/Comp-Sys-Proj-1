#include "solver.h"
#include "utils.h"

#define SEM_ID    250	 /* ID for the semaphore.               */

struct solution solver_oneprocess(int len, int* data){
	struct solution sol;
	float sum = 0.0;
	sol.max = 0;
	sol.num_hidden = 0;

	for(int i = 0; i < len; i++){
		if (data[i] >= 0){
			if (data[i] > sol.max){
				sol.max = (float)data[i];
			}
			sum += data[i];
		}
		else{
			sol.num_hidden += 1;
			// printf("Hi I'm process 0 and I found the hidden key in position %d\n", i);
		}
	}
	sol.avg = sum / (float)len;

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
    sol = (struct solution*) ((void*)shm_addr);

    /* fork-off a child process */
    pid = fork();
    switch (pid) {
	case -1:
	    perror("fork: ");
	    exit(1);
	    break;
	case 0:
	    do_child_DFS(len, data, NP, child_id, sem_set_id, sol);
	    exit(0);
	    break;
	default:
	    do_parent_DFS(len, data, NP, sem_set_id, sol);
	    break;
    }

    /* wait for child process's terination. */
	int child_status;
	wait(&child_status);

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

void do_child_DFS(int len, int* data, int NP, int child_id, int sem_set_id, struct solution* sol){
	// printf("Hi I'm process %d  and my parent is %d\n", getpid(), getppid());
	// check if want to spawn more children. first child always has child_id = 0
	// base case check!
	int pid = 0;
	if(!(child_id >= (NP-2))){
		// if not, spawn one child!
		pid = fork();
	    switch (pid) {
		case -1:
		    perror("fork: ");
		    exit(1);
		    break;
		case 0:
			// if you're a child, spawn another child if allowed, do work, and exit!
			child_id++;
		    do_child_DFS(len, data, NP, child_id, sem_set_id, sol);
		    exit(0);
		    break;
		default:
			// if you're parent, do nothing!
		    break;
	    }
	}

	// do some work
	// sleep(30);
	int max = 0;
	int sum = 0;
	int num_hidden = 0;
	int parts = len/NP;
	int mypid = getpid();
	for(int i = child_id*parts; i < (child_id+1)*parts; i++){
		if (data[i] >= 0){
			if (data[i] > max){
				max = data[i];
			}
			sum += data[i];
		}
		else{
			num_hidden += 1;
			// printf("Hi I'm process %d and I found the hidden key in position %d\n", mypid, i);
		}
	}
	sem_lock(sem_set_id);
	if(sol->max < max) sol->max = max;
	sol->avg += (float) sum;
	sol->num_hidden += num_hidden;
	sem_unlock(sem_set_id);

	if(pid != 0) { // if you spawnd another child. Take care of your child!
		int child_status;
        wait(&child_status);
	}
}

void do_parent_DFS(int len, int* data, int NP, int sem_set_id, struct solution* sol){
	// printf("Hi I'm the parent process with pid %d\n", getpid());
	// parent always processes the last segment!
	// sleep(30);
	int max = 0;
	int sum = 0;
	int num_hidden = 0;
	int parts = len/NP;
	int mypid = getpid();
	for(int i = (NP-1)*parts; i < len; i++){
		if (data[i] >= 0){
			if (data[i] > max){
				max = data[i];
			}
			sum += data[i];
		}
		else{
			num_hidden += 1;
			// printf("Hi I'm process %d and I found the hidden key in position %d\n", mypid, i);
		}
	}
	sem_lock(sem_set_id);
	if(sol->max < max) sol->max = max;
	sol->avg += (float) sum;
	sol->num_hidden += num_hidden;
	sem_unlock(sem_set_id);
}

struct solution solver_multiprocess(int len, int* data, int NP, int X){
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

    /* allocate the struct pointer and the num_processes to the shared memory */
    num_processes = (int*) shm_addr;
    *num_processes = 1;
    sol = (struct solution*) ((void*)shm_addr+sizeof(int));

    // base case check!
	int to_spawn;
	int current_p = 0;
	current_p = *num_processes;
	if(*num_processes >= NP){
		to_spawn = 0; //spawn no processes
	}
	else if((*num_processes + X) > NP){
		to_spawn = NP - *num_processes; //spawn the number of processes allowed
	}
	else{
		to_spawn = X; //spawn max number of processes
	}
	*num_processes += to_spawn;

	// printf("Hi I'm the parent process with pid %d. I will spawn %d children!\n", getpid(), to_spawn);
	for(int i = 0; i < to_spawn; i++){
	    pid = fork();
	    if(pid == -1){
		    perror("fork: ");
		    exit(1);
		}
		else if(pid == 0){
		    do_child_multiprocess(len, data, NP, X, child_id, sem_set_id, num_processes, sol);
		    exit(0);
	    }
	    else{
	    	child_id++;
	    }
	}
	// do some work
	do_parent_multiprocess(len, data, NP, X, sem_set_id, num_processes, sol);

    /* wait for child process's terination. */
	int child_status;
	for(int i = 0; i < to_spawn; i++){
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

void do_child_multiprocess(int len, int* data, int NP, int X, int child_id, int sem_set_id, int* num_processes, struct solution* sol){
	// base case check!
	int to_spawn;
	int current_p = 0;
	sem_lock(sem_set_id);
	current_p = *num_processes;
	if(*num_processes >= NP){
		to_spawn = 0; //spawn no processes
	}
	else if((*num_processes + X) > NP){
		to_spawn = NP - *num_processes; //spawn the number of processes allowed
	}
	else{
		to_spawn = X; //spawn max number of processes
	}
	*num_processes += to_spawn;
	sem_unlock(sem_set_id);

	// printf("Hi I'm process %d  and my parent is %d. I will spawn %d children!\n", getpid(), getppid(), to_spawn);
	
	int pid = 0;
	int grandchild_id = current_p - 1;
	if(to_spawn > 0){
		// if you're fertile, spawn children
		for(int i = 0; i < to_spawn; i++){
			pid = fork();
		    if(pid == -1){
			    perror("fork: ");
			    exit(1);
			}
			else if(pid == 0){
				// if you're a child, spawn another child if allowed, do work, and exit!
			    do_child_multiprocess(len, data, NP, X, grandchild_id, sem_set_id, num_processes, sol);
			    exit(0);
			}
			else{
				grandchild_id++;
			}
		}
	}

	// do some work
	// sleep(30);
	int max = 0;
	int sum = 0;
	int num_hidden = 0;
	int parts = len/NP;
	int mypid = getpid();
	for(int i = child_id*parts; i < (child_id+1)*parts; i++){
		if (data[i] >= 0){
			if (data[i] > max){
				max = data[i];
			}
			sum += data[i];
		}
		else{
			num_hidden += 1;
			// printf("Hi I'm process %d and I found the hidden key in position %d\n", mypid, i);
		}
	}
	sem_lock(sem_set_id);
	if(sol->max < max) sol->max = max;
	sol->avg += (float) sum;
	sol->num_hidden += num_hidden;
	sem_unlock(sem_set_id);

	if(pid != 0) { // if you spawned children. Take care of your children!
		int child_status;
		for(int i = 0; i < to_spawn; i++){
			wait(&child_status);
        }
	}
}

void do_parent_multiprocess(int len, int* data, int NP, int X, int sem_set_id, int* num_processes, struct solution* sol){

	// parent always processes the last segment!
	// sleep(30);
	int max = 0;
	int sum = 0;
	int num_hidden = 0;
	int parts = len/NP;
	int mypid = getpid();
	for(int i = (NP-1)*parts; i < len; i++){
		if (data[i] >= 0){
			if (data[i] > max){
				max = data[i];
			}
			sum += data[i];
		}
		else{
			num_hidden += 1;
			// printf("Hi I'm process %d and I found the hidden key in position %d\n", mypid, i);
		}
	}
	sem_lock(sem_set_id);
	if(sol->max < max) sol->max = max;
	sol->avg += (float) sum;
	sol->num_hidden += num_hidden;
	sem_unlock(sem_set_id);
}

int solver_onlyH(int len, int* data, int NP, int X, int H){
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
    int* num_Hfound;
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
    shm_id = shmget(100, 2*sizeof(int), IPC_CREAT | IPC_EXCL | 0600);
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

    /* allocate the struct pointer and the num_processes to the shared memory */
    num_processes = (int*) shm_addr;
    *num_processes = 1;
    num_Hfound = (int*) ((void*)shm_addr+sizeof(int));
    *num_Hfound = 0;

    // base case check!
	int to_spawn;
	int current_p = 0;
	current_p = *num_processes;
	if(*num_processes >= NP){
		to_spawn = 0; //spawn no processes
	}
	else if((*num_processes + X) > NP){
		to_spawn = NP - *num_processes; //spawn the number of processes allowed
	}
	else{
		to_spawn = X; //spawn max number of processes
	}
	*num_processes += to_spawn;

	// printf("Hi I'm the parent process with pid %d. I will spawn %d children!\n", getpid(), to_spawn);
	for(int i = 0; i < to_spawn; i++){
	    pid = fork();
	    if(pid == -1){
		    perror("fork: ");
		    exit(1);
		}
		else if(pid == 0){
		    do_child_onlyH(len, data, NP, X, H, child_id, sem_set_id, num_processes, num_Hfound);
		    exit(0);
	    }
	    else{
	    	child_id++;
	    }
	}
	// do some work
	do_parent_onlyH(len, data, NP, X, H, sem_set_id, num_processes, num_Hfound);

    /* wait for child process's terination. */
	int child_status;
	for(int i = 0; i < to_spawn; i++){
		wait(&child_status);
    }

    // copy the answers and post-process
    int myH;
    myH = *num_Hfound;

    /* detach the shared memory segment from our process's address space. */
    if (shmdt(shm_addr) == -1) {
        perror("main: shmdt: ");
    }

    /* de-allocate the shared memory segment. */
    if (shmctl(shm_id, IPC_RMID, &shm_desc) == -1) {
        perror("main: shmctl: ");
    }

	return myH;
}

void do_child_onlyH(int len, int* data, int NP, int X, int H, int child_id, int sem_set_id, int* num_processes, int* num_Hfound){
	// base case check!
	int to_spawn = 0;
	int current_p = 0;
	sem_lock(sem_set_id);
	if(*num_Hfound < H){
		current_p = *num_processes;
		if(*num_processes >= NP){
			to_spawn = 0; //spawn no processes
		}
		else if((*num_processes + X) > NP){
			to_spawn = NP - *num_processes; //spawn the number of processes allowed
		}
		else{
			to_spawn = X; //spawn max number of processes
		}
		*num_processes += to_spawn;
	}
	sem_unlock(sem_set_id);

	// printf("Hi I'm process %d  and my parent is %d. I will spawn %d children!\n", getpid(), getppid(), to_spawn);
	
	int pid = 0;
	int grandchild_id = current_p - 1;
	if(to_spawn > 0){
		// if you're fertile, spawn children
		for(int i = 0; i < to_spawn; i++){
			pid = fork();
		    if(pid == -1){
			    perror("fork: ");
			    exit(1);
			}
			else if(pid == 0){
				// if you're a child, spawn another child if allowed, do work, and exit!
			    do_child_onlyH(len, data, NP, X, H, grandchild_id, sem_set_id, num_processes, num_Hfound);
			    exit(0);
			}
			else{
				grandchild_id++;
			}
		}
	}

	// do some work
	// sleep(30);
	if(*num_Hfound < H){
		bool notallfound = true;
		int parts = len/NP;
		int mypid = getpid();
		for(int i = child_id*parts; (i < (child_id+1)*parts) && notallfound; i++){
			if (data[i] < 0){
				sem_lock(sem_set_id);
				*num_Hfound += 1;
				if(*num_Hfound >= H) notallfound = false;
				sem_unlock(sem_set_id);
				// printf("Hi I'm process %d and I found the hidden key in position %d\n", mypid, i);
			}
		}
	}

	if(pid != 0) { // if you spawned children. Take care of your children!
		int child_status;
		for(int i = 0; i < to_spawn; i++){
			wait(&child_status);
        }
	}
}

void do_parent_onlyH(int len, int* data, int NP, int X, int H, int sem_set_id, int* num_processes, int* num_Hfound){

	// parent always processes the last segment!
	// sleep(30);
	if(*num_Hfound < H){
		bool notallfound = true;
		int parts = len/NP;
		int mypid = getpid();
		for(int i = (NP-1)*parts; (i < len) && notallfound; i++){
			if (data[i] < 0){
				sem_lock(sem_set_id);
				*num_Hfound += 1;
				if(*num_Hfound >= H) notallfound = false;
				sem_unlock(sem_set_id);
				// printf("Hi I'm process %d and I found the hidden key in position %d\n", mypid, i);
			}
		}
	}
}