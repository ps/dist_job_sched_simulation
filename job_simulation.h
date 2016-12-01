#ifndef JOB_SIMULATION_H
#define JOB_SIMULATION_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stddef.h> // for NULL
#include <sys/time.h>
#include <unistd.h> // for sleep
#include <limits.h> // max int size

#define TRUE 1
#define FALSE 0

#define MAX_WORKER_QUEUE_CAPACITY 16

#define NODE_SELECT_SEQUENTIAL 111
#define NODE_SELECT_RANDOM 222
#define NODE_SELECT_SHORTEST_QUEUE 333

#define CONSTANT_JOB_ASSIGNMENT_STRATEGY 11
#define LINEAR_JOB_ASSIGNMENT_STRATEGY 22
#define EXPONENTIAL_JOB_ASSIGNMENT_STRATEGY 33

// this should not exceed MAX_WORKER_QUEUE_CAPACITY for obvious reasons
#define CONSTANT_JOB_CHUNK_SIZE 2

// used 
#define NUM_JOBS_TO_DISTRIBUTE 30

// defining function pointer with void return and no arguments provided
typedef void (*JobFunction)();

typedef struct JobNode {
    struct JobNode * next;
    JobFunction job_function;    
} JobNode;

typedef struct Jobs {
    int max_capacity;
    int size;
    int terminate;
    // end pointer
    JobNode * last_job;
    // front pointer
    JobNode * next_job;
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
    pthread_mutex_t jobs_lock;
    pthread_cond_t work_added;
} WorkerParams;


void print_log(Log * log);
void free_log(Log * log);
void log_message(Log * log, int msg_id);
long usecs();
double ms_to_sec(long ms);
void dummy_job();
void dummy_job2();
JobFunction remove_job(Jobs * jobs);
int add_job(Jobs * jobs, JobFunction job_function);
int add_jobs(Jobs * jobs, JobFunction * multiple_jobs, int num_to_add);
void * worker_node(void * params);
void init_worker_param(WorkerParams * worker_param, int thread_id);
void launch_master_node(int num_workers, int node_selection_strategy, int job_assignment_strategy);
#endif