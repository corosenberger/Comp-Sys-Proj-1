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
	    fprintf(stderr, "%s\n", "ERROR! Unable to convert argument to an integer");
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
	fp = fopen("data.txt", "w+");
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