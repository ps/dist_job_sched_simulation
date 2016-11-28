#ifndef JOB_SIMULATION_H
#define JOB_SIMULATION_H

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


void print_log(Log * log);
void free_log(Log * log);
void log_message(Log * log, int msg_id);
long usecs();
void dummy_job();
void dummy_job2();
JobFunction remove_job(Jobs * jobs);
void add_job(Jobs * jobs, JobFunction job_function);
void * worker_node(void * params);
void init_worker_param(WorkerParams * worker_param, int thread_id);
void launch_master_node(int num_workers);
#endif