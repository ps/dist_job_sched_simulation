#include "job_simulation.h"

// returns time since epoch in microseconds (10^6)
long usecs() {
    struct timeval t;
    gettimeofday(&t,NULL);
    return t.tv_sec*1000000+t.tv_usec;
}

double ms_to_sec(long ms) {
	return ms / 1000000.0;
}