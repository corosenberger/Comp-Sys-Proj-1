#include "utils.h"


#define HIDDEN_VAL -10 // value of hidden element 
#define MAXINT 1000 // maximum value of an integer to be put into file

int argvtoint(char* arg){
	// Source: https://stackoverflow.com/questions/9748393/how-can-i-get-argv-as-int/38669018
	char* p;
	int num;
	long conv = strtol(arg, &p, 10);
	errno = 0;
	// Check for errors: e.g., the string does not represent an integer
	// or the integer is larger than int
	if (errno != 0 || *p != '\0' || conv > INT_MAX || conv < INT_MIN) {
	    // Put here the handling of the error, like exiting the program with
	    // an error message
	    fprintf(stderr, "%s\n", "ERROR! Unable to convert input to an integer");
	    exit(1);
	} else {
	    // No error
	    num = conv;    
	}
	return num;
}

int data_generator(int L, int H){
	time_t t;
	srand((unsigned) time(&t));
	int* Hlist;
	int bucket_size = L/H; // size of a bucket for regular distribution

	if(H > 0){
		Hlist = malloc(H*sizeof(int));
		// RAND_MAX is around 2.14 billion, so we're good
		// This loop yields a sorted list of positions where we
		// put the Hidden elements
		// works pretty okay for H << L
		for(int i = 0; i < H; i++){
			Hlist[i] = (rand() % bucket_size) + i*bucket_size;
		}
	}

	int j = 0;
	FILE* fp;
	fp = fopen("data.txt", "w");
	for(int i = 0 ; i < L ; i++ ){
		if(i != Hlist[j]){
			fprintf(fp, "%d\n", rand() % MAXINT);
		}
		else{
			fprintf(fp, "%d\n", HIDDEN_VAL);
			j++;
		}
	}
	fclose(fp);
	free(Hlist);
}

void data_loader(int L, int* data){
	/// Source: https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
	FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int i = 0;
    char* posnum;

    fp = fopen("data.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
    	line[read-1] = '\0';
    	data[i] = argvtoint(line);
    	i++;
    }

	fclose(fp);
	if (line) free(line);
}

void print_sol(struct solution sol){
	printf("MAX: %d, AVG: %f, NUM_HIDDEN:%d\n\n", sol.max, sol.avg, sol.num_hidden);
}



/*
 * function: sem_lock. locks the semaphore, for exclusive access to a resource.
 * input:    semaphore set ID.
 * output:   none.
 */
void sem_lock(int sem_set_id) {
    /* structure for semaphore operations.   */
    struct sembuf sem_op;

    /* wait on the semaphore, unless it's value is non-negative. */
    sem_op.sem_num = 0;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;
    semop(sem_set_id, &sem_op, 1);
}

/*
 * function: sem_unlock. un-locks the semaphore.
 * input:    semaphore set ID.
 * output:   none.
 */
void sem_unlock(int sem_set_id) {
    /* structure for semaphore operations.   */
    struct sembuf sem_op;

    /* signal the semaphore - increase its value by one. */
    sem_op.sem_num = 0;
    sem_op.sem_op = 1;   /* <-- Comment 3 */
    sem_op.sem_flg = 0;
    semop(sem_set_id, &sem_op, 1);
}