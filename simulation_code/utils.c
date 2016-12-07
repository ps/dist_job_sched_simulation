#include "job_simulation.h"

// for now, there wouldn't seem to be a point of running this app with 5000 threads 
unsigned int seed[5001] = {0,0,0,0};

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
	if(seed[thread_id+1] == 0) {
		seed[thread_id+1] = time(NULL) + thread_id + 1;
	}
	//unsigned int thread_seed = seed + thread_id;
	//printf("thread seeed: %i\n", seed[thread_id + 1]);
	return rand_r(&seed[thread_id + 1]);
}

unsigned long estimate_sequential_timing(int num_small, int num_mid, int num_large) {
    unsigned long start = usecs();
    sample_job(SMALL_JOB);
    unsigned long end = usecs();
    unsigned long diff = (end - start) * num_small;

    start = usecs();
    sample_job(MID_JOB);
    end = usecs();
    diff += (end - start) * num_mid;

    start = usecs();
    sample_job(LARGE_JOB);
    end = usecs();
    diff += (end - start) * num_large;

    return diff;
}