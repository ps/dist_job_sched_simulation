#include "job_simulation.h"

static unsigned int seed = 0;

// returns time since epoch in microseconds (10^6)
long usecs() {
    struct timeval t;
    gettimeofday(&t,NULL);
    return t.tv_sec*1000000+t.tv_usec;
}

double ms_to_sec(long ms) {
	return ms / 1000000.0;
}

int get_rand(int thread_id) {
	if(seed == 0) {
		seed = time(NULL);
	}
	unsigned int thread_seed = seed + thread_id;
	return rand_r(&thread_seed);
}