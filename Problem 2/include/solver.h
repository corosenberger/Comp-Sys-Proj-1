#ifndef SOLVER__H
#define SOLVER__H

struct solution
{
	int max;
	float avg;
	int num_hidden;
};

struct solution solver_oneprocess(int len, int* data); //generates the solution with only one process
struct solution solver_DFS(int len, int* data, int NP); //generates solution with multiple processes

void do_child_DFS(int len, int* data, int NP, int child_id, int sem_set_id, int* num_processes, struct solution* sol);
void do_parent_DFS(int len, int* data, int NP, int sem_set_id, int* num_processes, struct solution* sol);

#endif