#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stddef.h> // for NULL
#include <sys/time.h>
#include <unistd.h> // for sleep

#define TRUE 1
#define FALSE 0

// defining function pointer with void return and no arguments provided
typedef void (*JobFunction)();

typedef struct JobNode {
    struct JobNode * next;
    JobFunction job_function;    
} JobNode;

typedef struct Jobs {
    int size;
    int terminate;
    // end pointer
    JobNode * last_job;
    // front pointer
    JobNode * next_job;
    pthread_mutex_t jobs_lock;
    pthread_cond_t work_added;
} Jobs;

typedef struct LogNode {
    struct LogNode * next;
    int msg_id;
    unsigned long timestamp;
} LogNode;

typedef struct Log {
    pthread_mutex_t log_lock;
    LogNode * log_msg;
} Log;

typedef struct WorkerParams {
    int thread_id;
    Log * log;
    Jobs * jobs;
} WorkerParams;


void * worker_node(void * params);
void launch_master_node();
long usecs();

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


void print_log(Log * log) {
    LogNode * log_node = log->log_msg;
    while(log_node != NULL) {
        printf("msg: %i timestamp: %lu\n", log_node->msg_id, log_node->timestamp);
        log_node = log_node->next;
    }
}

void free_log(Log * log) {
    pthread_mutex_destroy(&log->log_lock);
    LogNode * log_node = log->log_msg;
    while(log_node != NULL) {
        LogNode * temp = log_node->next;
        free(log_node);
        log_node = temp;
    }
    free(log);
}

void log_message(Log * log, int msg_id) {
    LogNode * log_node = (LogNode *)malloc(sizeof(LogNode));
    log_node->msg_id = msg_id;
    log_node->timestamp = usecs();

    pthread_mutex_lock(&log->log_lock);
    log_node->next = log->log_msg;
    log->log_msg = log_node;
    pthread_mutex_unlock(&log->log_lock);
}

// IMPORTANT: make sure job lock is held before using this function
// Adds to end of the Jobs list
void add_job(Jobs * jobs, JobFunction job_function) {
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


long usecs() {
    struct timeval t;
    gettimeofday(&t,NULL);
    return t.tv_sec*1000000+t.tv_usec;
}

void * worker_node(void * params) {
    WorkerParams * my_params = (WorkerParams *) params;
    int thread_id = my_params->thread_id;
    Log * log = my_params->log;
    Jobs * jobs = my_params->jobs;
    pthread_mutex_t jobs_lock = jobs->jobs_lock;
    while(TRUE) {
        pthread_mutex_lock(&jobs_lock);
        printf("Node id %i waiting for jobs.\n", thread_id);
        while(jobs->size == 0 && jobs->terminate == FALSE) {
            pthread_cond_wait(&jobs->work_added, &jobs_lock);
        }
        int terminate = jobs->terminate;
        JobFunction job = remove_job(jobs);
        int num_jobs_remaining = jobs->size;
        pthread_mutex_unlock(&jobs_lock);
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

void init_worker_param(WorkerParams * worker_param, int thread_id) {
    Log * log = (Log *)malloc(sizeof(Log));
    log->log_msg = NULL;
    pthread_mutex_init(&log->log_lock, NULL);
    
    Jobs * jobs = (Jobs *)malloc(sizeof(Jobs));
    jobs->size = 0;
    jobs->terminate = FALSE;
    jobs->next_job = NULL;
    jobs->last_job = NULL;
    pthread_mutex_init(&jobs->jobs_lock, NULL);
    pthread_cond_init(&jobs->work_added, NULL);

    worker_param->jobs = jobs;
    worker_param->log = log;
    worker_param->thread_id = thread_id;
}

void launch_master_node(int num_workers) {
    pthread_t * worker = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
    WorkerParams * worker_params = (WorkerParams *)malloc(sizeof(WorkerParams) * num_workers);
    printf("Master about to launch slaves.\n"); 
    int i;
    for(i = 0; i < num_workers; i++) {
        init_worker_param(&worker_params[i], i);
        pthread_create(&worker[i], NULL, &worker_node, (void *)&worker_params[i]);
    }

    for(i = 0; i < num_workers; i++) {
        Jobs * jobs = worker_params[i].jobs;
        printf("adding 2 jobs to id %i\n", worker_params[i].thread_id);
        pthread_mutex_lock(&jobs->jobs_lock);
        add_job(jobs, &dummy_job);
        add_job(jobs, &dummy_job);
        pthread_cond_signal(&jobs->work_added);
        pthread_mutex_unlock(&jobs->jobs_lock);
        int sleep_time = 3;
        printf("master sleeping for %i sec.\n", sleep_time);
        //sleep(sleep_time);
        printf("master awoke and adding one more job and telling thread to die\n");
        pthread_mutex_lock(&jobs->jobs_lock);
        add_job(jobs, &dummy_job);
        jobs->terminate = TRUE;
        pthread_cond_signal(&jobs->work_added);
        pthread_mutex_unlock(&jobs->jobs_lock);
    }
    

    printf("Master joining on workers.\n");
    for(i = 0; i < num_workers; i++) {
        pthread_join(worker[i], NULL);
        printf("Printing log for thread id %i\n", i);
        print_log(worker_params[i].log);
        free_log(worker_params[i].log);
    }
    
    free(worker);
    free(worker_params);
    printf("Master exiting.\n");
}


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
