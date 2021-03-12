#ifndef UTILS__H
#define UTILS__H

#include <limits.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int argvtoint(char* arg); //function to convert argument values into integer
int data_generator(int L, int H); //function to output data into data.txt

#endif