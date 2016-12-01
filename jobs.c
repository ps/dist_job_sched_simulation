#include "job_simulation.h"

void sample_job(double job_scale) {
	job_scale = abs(job_scale);
	if(job_scale > 1.0) {
		printf("WARNING: Invalid job size scale, rescaling to 1. Value must be contained in interval [0,1]\n");
		job_scale = 1.0;
	}
    int i;
    double k = 34.5;
    double z = -23.2;
    int loop_bound = (int) INT_MAX * job_scale;
    for(i = 0; i < loop_bound; i++) {
        k = k / z * k;
    }
}