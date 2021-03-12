#ifndef SOLVER__H
#define SOLVER__H

#include <stdio.h>
#include <stdlib.h>

struct solution
{
	int max;
	float avg;
	int num_hidden;
};

struct solution solver_oneprocess(int len, int* data); //generates the solution with only one process

#endif