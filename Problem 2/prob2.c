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


	printf("Generating Data with L = %d, and H = %d. . . .\n\n",  L, H);
	data_generator(L, H);

	int* data = malloc(L*sizeof(int));
	printf("Loading Data from data.txt . . . .\n\n");
	data_loader(L, data);

	printf("Solving with One Process . . . .\n");
	results = solver_oneprocess(L, data);
	print_sol(results);

	printf("Solving with DFS Approach with NP = %d . . . .\n", NP);
	results = solver_DFS(L, data, NP);
	print_sol(results);

	printf("Solving with Multi-Process Approach with NP = %d, X = %d . . . .\n", NP, X);
	results = solver_multiprocess(L, data, NP, X);
	print_sol(results);

	free(data);
	return 0;
}	