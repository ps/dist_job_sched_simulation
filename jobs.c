#include "job_simulation.h"

void clear_array(double * data, int size) {
	int i;
	for(i = 1; i < size; i++) {
		data[i] = data[i - 1];
	}
}

void sample_job(double job_scale) {
	job_scale = abs(job_scale);
	if(0 && job_scale > 1.0) {
		printf("WARNING: Invalid job size scale, rescaling to 1. Value must be contained in interval [0,1]\n");
		job_scale = 1.0;
	}
	// 4 * 100000 ~ 0.4mb
	// 4 * 2147483 ~ 8.6mb
	//int array_size = 2147483 * job_scale; 
	int array_size = 100000; 
	int iterations = 2000 - job_scale;
	double * data = (double *)malloc(sizeof(double) * array_size);
	clear_array(data, array_size);
    volatile int i, k;
    volatile double q = 34.5 * job_scale;
    volatile double z = -23.2 + job_scale;
    //volatile int loop_bound = (int) INT_MAX * job_scale;
    for(i = 0; i < iterations; i++) {
		for(k = 1; k < array_size; k++) {
			asm("");
    		q = q / z * q;
    		data[k] = q + 1.2;
    		data[k - 1] = data[k] * data[k];
		}
    }
    free(data);
}