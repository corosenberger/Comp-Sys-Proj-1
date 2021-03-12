#ifndef UTILS__H
#define UTILS__H

#include <limits.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "solver.h"

int argvtoint(char* arg); //function to convert argument values into integer
int data_generator(int L, int H); //function to output data into data.txt
void data_loader(int L, int* data); //function to load data from data.txt
void print_sol(struct solution); // function to print results

#endif