#ifndef UTILS__H
#define UTILS__H

#include <limits.h>
#include <errno.h>
#include <time.h>
#include <wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>     /* general SysV IPC structures          */
#include <sys/sem.h>	 /* semaphore functions and structs.     */
#include <sys/shm.h>	 /* shared memory functions and structs. */
#include <stdbool.h>
#include <sys/time.h>
#include "solver.h"

int argvtoint(char* arg); //function to convert argument values into integer
int data_generator(int L, int H); //function to output data into data.txt
void data_loader(int L, int* data); //function to load data from data.txt
void print_sol(struct solution); // function to print results

// semaphore functions. Source: http://www.cs.kent.edu/~ruttan/sysprog/lectures/shmem/shared-mem-with-semaphore.c
void sem_lock(int sem_set_id);
void sem_unlock(int sem_set_id);

#endif