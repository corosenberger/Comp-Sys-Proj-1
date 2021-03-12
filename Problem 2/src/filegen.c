#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

/*
This file works by having two arguments other than filename, and generates text file 
required to be processed in this problem 2

Usage: ./filegen L H
L denotes the length (number of numbers) of the text file generated ("data.txt")
H denotes the number of Hidden elements uniformly distributed in the text file
*/

int argvtoint(char* arg); // helper function to convert argument values to integers

int main(int argc, char* argv[]){

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

	data_generator(L, H);
	return 0;
}	