#include "job_simulation.h"

void * worker_node(void * params) {
    WorkerParams * my_params = (WorkerParams *) params;
    int thread_id = my_params->thread_id;
    Log * log = my_params->log;
    pthread_mutex_t * jobs_lock = &my_params->jobs_lock;
    pthread_cond_t * work_added = &my_params->work_added;

    log_message(log, START_PROCESSING_MSG, NO_DATA);
    while(TRUE) {
        pthread_mutex_lock(jobs_lock);
        Jobs * jobs = my_params->jobs;
        int show = FALSE;
        while(jobs->size == 0 && jobs->terminate == FALSE) {
            if(!show) {
                show = TRUE;
            }
            pthread_cond_wait(work_added, jobs_lock);
        }
        // NOTE: If concurrency issues happen to occur, see if acquiring then releasing a 
        // second lock (as is done inside log_message) is causing trouble
        log_message(log, WORKER_QUEUE_SIZE_MSG, jobs->size);
        
        int terminate = jobs->terminate;
        JobData job_data = remove_job(jobs);
        int num_jobs_remaining = jobs->size;
        pthread_mutex_unlock(jobs_lock);

        if(job_data.empty == FALSE) {
            JobFunction job = job_data.job_function;
            job(job_data.job_parameter);
        } else {
            if(num_jobs_remaining != 0) {
                printf("UNEXPECTED ERROR: Node %i has NULL job with queue size %i\n", thread_id, num_jobs_remaining);
                pthread_exit((void *)NULL); 
            }
        }

        if(num_jobs_remaining == 0 && terminate == TRUE) {
            break;
        }
    }
    log_message(log, END_PROCESSING_MSG, NO_DATA);

    pthread_exit((void *)NULL);
}