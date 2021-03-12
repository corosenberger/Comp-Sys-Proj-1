#include "solver.h"

struct solution solver_oneprocess(int len, int* data){
	struct solution sol;
	int sum = 0;
	sol.max = 0;
	sol.num_hidden = 0;

	for(int i = 0; i < len; i++){
		if (data[i] > 0){
			if (data[i] > sol.max){
				sol.max = data[i];
			}
			sum += data[i];
		}
		else{
			sol.num_hidden += 1;
			printf("Hi I'm process 0 and I found the hidden key in position %d\n", i);
		}
	}
	sol.avg = (float)sum / (float)len;

	return sol;
}