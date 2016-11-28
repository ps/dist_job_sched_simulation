#include "job_simulation.h"


int main(int argc, char ** argv) {
    int num_workers = 30;
    JobFunction job = &dummy_job;
    job();
    JobFunction job2 = &dummy_job2;
    job2();

/*    Jobs * jobs = (Jobs *)malloc(sizeof(Jobs));
    jobs->size = 0;
    jobs->terminate = FALSE;
    int i;
    for(i = 0; i < 6; i++) {
        add_job(jobs, &dummy_job2);
        printf("jobs size: %i\n", jobs->size);
    }
    JobFunction to_execute;
    while((to_execute = remove_job(jobs)) != NULL) {
        printf("jobs size upon removing: %i\n", jobs->size);
        to_execute();
    }*/


    launch_master_node(num_workers);
}
