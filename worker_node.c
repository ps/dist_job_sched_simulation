#include "job_simulation.h"

void * worker_node(void * params) {
    WorkerParams * my_params = (WorkerParams *) params;
    int thread_id = my_params->thread_id;
    Log * log = my_params->log;
    pthread_mutex_t * jobs_lock = &my_params->jobs_lock;
    pthread_cond_t * work_added = &my_params->work_added;
    printf("I am node id %i\n", thread_id);
    while(TRUE) {
        pthread_mutex_lock(jobs_lock);
        Jobs * jobs = my_params->jobs;
        printf("Node id %i waiting for jobs.\n", thread_id);
        while(jobs->size == 0 && jobs->terminate == FALSE) {
            pthread_cond_wait(work_added, jobs_lock);
        }
        int terminate = jobs->terminate;
        JobFunction job = remove_job(jobs);
        int num_jobs_remaining = jobs->size;
        pthread_mutex_unlock(jobs_lock);
        printf("Node id %i received job, about to process\n", thread_id);
        job();
        printf("Node id %i FINISHED JOB\n", thread_id);
        if(num_jobs_remaining == 0 && terminate == TRUE) {
            break;
        }
    }

    printf("Node id %i finished processing jobs and received terminate signal.\n", thread_id);
    int i;
    for(i = 0; i < thread_id + 1; i++) {
        log_message(log, 1 + thread_id);
    }
    pthread_exit((void *)NULL);
}