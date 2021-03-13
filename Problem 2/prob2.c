#include "utils.h"
#include "solver.h"

/*
This file works by having three arguments other than filename, and runs the problems
stated in Problem 2.

Usage: ./prob2 L H NP
L denotes the length (number of numbers) of the text file generated ("data.txt")
H denotes the number of Hidden elements uniformly distributed in the text file
NP denotes the maximum number of processes to be created.
*/

int main(int argc, char* argv[]){

	struct solution results;

	if(argc < 5 || argc > 5){
		fprintf(stderr, "%s\n", "ERROR: The number of arguments should be 4");
		exit(1);
	}

	int L = argvtoint(argv[1]);
	int H = argvtoint(argv[2]);
	int NP = argvtoint(argv[3]);
	int X = argvtoint(argv[4]);
	if(H > L){
		fprintf(stderr, "%s\n", "ERROR: The number of hidden elements cannot be greater than file length");
		exit(1);
	}
	else if(X > NP){
		fprintf(stderr, "%s\n", "ERROR: X cannot be greater than NP");
		exit(1);
	}

	struct timeval  tv1, tv2;
	double cpu_time_used;	


	printf("Generating Data with L = %d, and H = %d. . . .\n\n",  L, H);
	data_generator(L, H);

	int* data = malloc(L*sizeof(int));
	printf("Loading Data from data.txt . . . .\n\n");
	data_loader(L, data);

	printf("Solving with One Process . . . .\n");
	gettimeofday(&tv1, NULL);
	results = solver_oneprocess(L, data);
	gettimeofday(&tv2, NULL);
	cpu_time_used = (double)(tv2.tv_sec - tv1.tv_sec) + (double) (tv2.tv_usec - tv1.tv_usec) / 1000000;
	printf("TIME TAKEN: %.8f\n", cpu_time_used);
	print_sol(results);
	

	printf("Solving with DFS Approach with NP = %d . . . .\n", NP);
	gettimeofday(&tv1, NULL);
	results = solver_DFS(L, data, NP);
	gettimeofday(&tv2, NULL);
	cpu_time_used = (double)(tv2.tv_sec - tv1.tv_sec) + (double) (tv2.tv_usec - tv1.tv_usec) / 1000000;
	printf("TIME TAKEN: %.8f\n", cpu_time_used);
	print_sol(results);
	

	printf("Solving with Multi-Process Approach with NP = %d, X = %d . . . .\n", NP, X);
	gettimeofday(&tv1, NULL);
	results = solver_multiprocess(L, data, NP, X);
	gettimeofday(&tv2, NULL);
	cpu_time_used = (double)(tv2.tv_sec - tv1.tv_sec) + (double) (tv2.tv_usec - tv1.tv_usec) / 1000000;
	printf("TIME TAKEN: %.8f\n", cpu_time_used);
	print_sol(results);

	int num_hidden = 0;
	printf("Only Finding H with NP = %d, X = %d . . . .\n", NP, X);
	gettimeofday(&tv1, NULL);
	num_hidden = solver_onlyH(L, data, NP, X, H);
	gettimeofday(&tv2, NULL);
	cpu_time_used = (double)(tv2.tv_sec - tv1.tv_sec) + (double) (tv2.tv_usec - tv1.tv_usec) / 1000000;
	printf("TIME TAKEN: %.8f\n", cpu_time_used);
	printf("NUM_HIDDEN: %d\n\n", num_hidden);

	free(data);
	return 0;
}	