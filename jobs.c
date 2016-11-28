#include "job_simulation.h"

void dummy_job() {
    int i;
    double k = 34.5;
    double z = -23.2;
    printf("executing dummy job 1\n");
    for(i = 0; i < 3000; i++) {
        k = k / z * k;
    }
}
void dummy_job2() {
    printf("exectuing dummy 2\n");
}