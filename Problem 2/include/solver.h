#ifndef SOLVER__H
#define SOLVER__H

struct solution
{
	int max;
	float avg;
	int num_hidden;
};

struct solution solver_oneprocess(int len, int* data); //generates the solution with only one process
struct solution solver_DFS(int len, int* data, int NP); //generates solution with DFS type approach
struct solution solver_multiprocess(int len, int* data, int NP, int X); //generates solution with multiple processes
int solver_onlyH(int len, int* data, int NP, int X, int H); //finds out where hidden elements are. knows H

void do_child_DFS(int len, int* data, int NP, int child_id, int sem_set_id, struct solution* sol);
void do_parent_DFS(int len, int* data, int NP, int sem_set_id, struct solution* sol);
void do_child_multiprocess(int len, int* data, int NP, int X, int child_id, int sem_set_id, int* num_processes, struct solution* sol);
void do_parent_multiprocess(int len, int* data, int NP, int X, int sem_set_id, int* num_processes, struct solution* sol);
void do_child_onlyH(int len, int* data, int NP, int X, int H, int child_id, int sem_set_id, int* num_processes, int* num_Hfound);
void do_parent_onlyH(int len, int* data, int NP, int X, int H, int sem_set_id, int* num_processes, int* num_Hfound);


#endif