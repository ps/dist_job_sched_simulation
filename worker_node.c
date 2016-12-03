#include "job_simulation.h"

void * worker_node(void * params) {
    WorkerParams * my_params = (WorkerParams *) params;
    int thread_id = my_params->thread_id;
    Log * log = my_params->log;
    pthread_mutex_t * jobs_lock = &my_params->jobs_lock;
    pthread_cond_t * work_added = &my_params->work_added;
    //printf("Node %i spawned.\n", thread_id);

    log_message(log, START_PROCESSING_MSG, NO_DATA);
    while(TRUE) {
        pthread_mutex_lock(jobs_lock);
        Jobs * jobs = my_params->jobs;
        int show = FALSE;
        while(jobs->size == 0 && jobs->terminate == FALSE) {
            if(!show) {
                show = TRUE;
                //printf("Node id %i waiting for jobs.\n", thread_id);
            }
            pthread_cond_wait(work_added, jobs_lock);
        }
        // NOTE: IF THERE ARE ISSUES THIS COULD BE THE CAUSE
        log_message(log, WORKER_QUEUE_SIZE_MSG, jobs->size);
        
        //printf("Node id %i job queue %i\n", thread_id, jobs->size);
        int terminate = jobs->terminate;
        JobData job_data = remove_job(jobs);
        int num_jobs_remaining = jobs->size;
        pthread_mutex_unlock(jobs_lock);

        if(job_data.empty == FALSE) {
            JobFunction job = job_data.job_function;
            //printf("Node id %i received job, about to process\n", thread_id);
            job(job_data.job_parameter);
            //printf("Node id %i FINISHED JOB\n", thread_id);
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

    //printf("Node id %i finished processing jobs and received terminate signal.\n", thread_id);

    pthread_exit((void *)NULL);
}