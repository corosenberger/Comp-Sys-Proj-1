#ifndef _PROBLEM_ONE_H
#define _PROBLEM_ONE_H

#include <stdlib.h>
#include <stdio.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char const *argv[]);
void CopyFile(int fd, const char *file_in);
void WriteInFile(int fd, const char *buff, int len);

#endif