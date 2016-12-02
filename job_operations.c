#include "job_simulation.h"

double get_job_scale_factor(int job_type, int thread_id) {
    double job_scales[3];
    job_scales[0] = LARGE_JOB;
    job_scales[1] = MID_JOB;
    job_scales[2] = SMALL_JOB;

    if(job_type == VARIED_JOB) {
        int index = get_rand(thread_id) % 3;
        return job_scales[index];
    } else if (job_type == LARGE_JOB) {
        return job_scales[0];
    } else if (job_type == MID_JOB) {
        return job_scales[1];
    } else if (job_type == SMALL_JOB) {
        return job_scales[2];
    }
    // if incorrect scale passed, give small job
    return job_scales[2];
}

JobData * generate_job_nodes(int num, int job_type, int thread_id) {
    JobData * jobs = (JobData *)malloc(sizeof(JobData) * num);
    int i;
    for(i = 0; i < num; i++) {
        jobs[i].job_parameter = get_job_scale_factor(job_type, thread_id);
        jobs[i].job_function = &sample_job;
        jobs[i].empty = FALSE;
    }
    return jobs;
}

// IMPORTANT: make sure job lock is held before using this function
int add_jobs(Jobs * jobs, JobData * multiple_jobs, int num_to_add) {
    int num_spots_available = jobs->max_capacity - jobs->size;
    if(num_spots_available < num_to_add) {
        return FALSE;
    }
    int i;
    for(i = 0; i < num_to_add; i++) {
        int added = add_job(jobs, multiple_jobs[i]);
        if(added == FALSE) {
            printf("UNEXPECTED ERROR: Job was not added.");
            return FALSE;
        }
    }
    return TRUE;
}

// IMPORTANT: make sure job lock is held before using this function
// Adds to end of the Jobs list
// returns FALSE if job was not added, TRUE otherwise
int add_job(Jobs * jobs, JobData job_data) {
    // max capacity reached can't add more jobs
    if(jobs->size == jobs->max_capacity) {
        return FALSE;
    }
    JobNode * new_job = (JobNode *)malloc(sizeof(JobNode));
    new_job->job_data = job_data;


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
JobData remove_job(Jobs * jobs) {
    JobData removed_job;
    removed_job.empty = TRUE;
    if(jobs->size != 0) {
        // fetch job node to return
        JobNode * job_node = jobs->next_job;

        // remove job from job list
        jobs->next_job = jobs->next_job->next;
        // no more jobs, readjust last_job pointer accordingly
        if(jobs->next_job == NULL) {
            jobs->last_job = NULL;
        }

        // adjust size
        jobs->size--;

        removed_job = job_node->job_data;

        free(job_node);
    }
    return removed_job;
}