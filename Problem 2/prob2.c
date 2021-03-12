#include "utils.h"
#include "solver.h"

/*
This file works by having two arguments other than filename, and generates text file 
required to be processed in this problem 2

Usage: ./prob2 L H
L denotes the length (number of numbers) of the text file generated ("data.txt")
H denotes the number of Hidden elements uniformly distributed in the text file
*/

int main(int argc, char* argv[]){

	struct solution results;

	if(argc < 3 || argc > 3){
		fprintf(stderr, "%s\n", "ERROR: The number of arguments should be 3");
		exit(1);
	}

	int L = argvtoint(argv[1]);
	int H = argvtoint(argv[2]);
	if(H > L){
		fprintf(stderr, "%s\n", "ERROR: The number of hidden elements cannot be greater than file length");
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

	free(data);
	return 0;
}	