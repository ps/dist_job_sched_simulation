#include "job_simulation.h"

// IMPORTANT: make sure job lock is held before using this function
// Adds to end of the Jobs list

// returns FALSE if job was not added, TRUE otherwise
int add_job(Jobs * jobs, JobFunction job_function) {
    // max capacity reached can't add more jobs
    if(jobs->size == jobs->max_capacity) {
        return FALSE;
    }
    JobNode * new_job = (JobNode *)malloc(sizeof(JobNode));
    new_job->job_function = job_function;

    new_job->next = NULL;
    if(jobs->last_job != NULL) {
        jobs->last_job->next = new_job;
    }
    jobs->last_job = new_job;

    if(jobs->next_job == NULL) {
        jobs->next_job = new_job;
    }
    jobs->size++;
    return TRUE;
}

// IMPORTANT: make sure job lock is held before using this function
JobFunction remove_job(Jobs * jobs) {
    if(jobs->size != 0) {
        // fetch job node to return
        JobNode * job_node = jobs->next_job;

        // remove job from job list
        jobs->next_job = jobs->next_job->next;
        // no more jobs, readjust last_job pointer accordingly
        if(jobs->next_job == NULL) {
            jobs->last_job = NULL;
        }

        JobFunction job_function = job_node->job_function;
        free(job_node);
        jobs->size--;
        return job_function;
    }
    return NULL;
}