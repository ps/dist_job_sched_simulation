#ifndef JOB_SIMULATION_H
#define JOB_SIMULATION_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
/*
    Needed for sched_yield()
*/
#include <sched.h>
/*
    Defines NULL macro
*/
#include <stddef.h> 
#include <sys/time.h>
#include <time.h>
/*
    For integer limits
*/
#include <limits.h> 

/***************************
**********CONSTANTS*********
****************************/
#define TRUE 1
#define FALSE 0

/*
    Defines whether log printed to stdout vs file.
*/
#define PRINT_LOG_TO_STDOUT FALSE

/*
    Since worker threads are indexed 0 through n, use -1 for master thread 
*/
#define MASTER_THREAD_ID -1

/*
    Used to specify that there is no data.
*/
#define NO_DATA INT_MIN

/*
    Node selection strategies.
*/
#define NODE_SELECT_SEQUENTIAL 111
#define NODE_SELECT_RANDOM 222
#define NODE_SELECT_SHORTEST_QUEUE 333

/*
    Job assignment strategies.
*/
#define CONSTANT_JOB_ASSIGNMENT_STRATEGY 11
#define LINEAR_JOB_ASSIGNMENT_STRATEGY 22
#define EXPONENTIAL_JOB_ASSIGNMENT_STRATEGY 33

/*
    Job types.
*/
#define LARGE_JOB 0
#define MID_JOB 1000
#define SMALL_JOB 1500
#define VARIED_JOB 66

/*
    Log message identificators.
*/

/*
    Stores the assignment rate/size at current timestamp.
*/
#define JOB_ASSIGNMENT_RATE_MSG 444
/*
    Stores time at which master/worker started processing.
*/
#define START_PROCESSING_MSG 555
/*
    Stores time at wchich master/worker ended processing.
*/
#define END_PROCESSING_MSG 666
/*
    Stores the queue size of worker at current timestamp.
*/
#define WORKER_QUEUE_SIZE_MSG 777
/*
    Stores the sleep time in microseconds of worker.
*/
#define WORKER_SLEEP_TIME_MSG 888
/*
    Stores the number of jobs remaining to distribute.
*/
#define JOBS_REMAINING_MSG 999
/***************************
**********CONSTANTS*********
****************************/


/*****************************************
**********CONFIGURATION VARIABLES*********
******************************************/
/*
    Specifies the number of jobs to distribute.
*/
extern int NUM_JOBS_TO_DISTRIBUTE;

/*
    Specifies the job type to be used:
    -ALL SMALL
    -ALL MID
    -ALL LARGE
    -VARIED
*/
extern int JOB_TYPE;

/*
    Specifies the number of workers, max 5000
*/
extern int NUM_WORKERS;

/*
    Specifies the limit on the maximum jobs allowed to be held by
    a worker queue.
*/
extern int MAX_WORKER_QUEUE_CAPACITY;

/*
    Specifies the maximum number of jobs master thread is allowed 
    to assign to worker. This should not be greater than 
    MAX_WORKER_QUEUE_CAPACITY.
*/
extern int MAX_JOBS_ASSIGNMENT;

/*
    Specifies the strategy used to select worker node:
    -SEQUENTIAL
    -RANDOM
    -SHORTEST QUEUE FIRST
*/
extern int NODE_SELECTION_STRATEGY;

/*
    Specifies the strategy used to distribute jobs:
    -CONSTANT
    -LINEAR
    -EXPONENTIAL
*/
extern int JOB_DISTRIBUTION_STRATEGY;

/*
    If constant assignment strategy specified, this defines
    the constant size to be used. This should not be greater 
    than MAX_WORKER_QUEUE_CAPACITY.
*/
extern int CONSTANT_JOB_CHUNK_SIZE;

/*****************************************
**********CONFIGURATION VARIABLES*********
******************************************/

/*****************************************
******************STRUCTS*****************
******************************************/

// defining function pointer with void return and no arguments provided
typedef void (*JobFunction)(int);

typedef struct JobData {
    int empty;
    int job_parameter;
    JobFunction job_function;    
} JobData;

typedef struct JobNode {
    struct JobNode * next;
    JobData job_data;
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
    int data;
    int msg_id;
    unsigned long timestamp;
} LogNode;

typedef struct Log {
    pthread_mutex_t log_lock;
    LogNode * log_msg;
    unsigned long first_log_timestamp;
    unsigned long last_log_timestamp;
} Log;

typedef struct WorkerParams {
    int thread_id;
    Log * log;
    Jobs * jobs;
    pthread_mutex_t jobs_lock;
    pthread_cond_t work_added;
} WorkerParams;
/*****************************************
******************STRUCTS*****************
******************************************/

/*****************************************
**************LOG OPERATIONS HEADERS******
******************************************/
/*
    Prints data in the provided log.
*/
void print_log(Log * log, int thread_id, unsigned long relative_start);

/*
    Initializes log.
*/
Log * init_log();

/*
    Frees log.
*/
void free_log(Log * log);

/*
    Logs message.
*/
void log_message(Log * log, int msg_id, int data);
/*****************************************
**************LOG OPERATIONS HEADERS******
******************************************/

/*****************************************
**************JOB OPERATIONS HEADERS******
******************************************/
/*
    After simulation is ran, use this function to 
    get the data on how many times each job type was
    ran. This is mainly serves the purpose to figure out
    how many types of each job VARIED setting ran.
*/
int get_job_frequency(int job_type);

/*
    Add one job to provided queue. 
    IMPORANT! Make sure job lock is held before calling this function.
*/
int add_job(Jobs * jobs, JobData job_data);

/*
    Add multiple jobs to provided queue. 
    IMPORANT! Make sure job lock is held before calling this function.
*/
int add_jobs(Jobs * jobs, JobData * multiple_jobs, int num_to_add);

/*
    Initialize jobs inside of WorkerParams. Used by master thread
    to setup worker threads.
*/
Jobs * init_jobs(WorkerParams * worker_param);

/*
    Removes job from queue. Used by workers to get a job from
    their queue.
*/
JobData remove_job(Jobs * jobs);

/*
    Generates jobs based on the provided parameters.
*/
JobData * generate_job_nodes(int num, int job_type, int thread_id);
/*****************************************
**************JOB OPERATIONS HEADERS******
******************************************/

/*****************************************
************UTILS OPERATIONS HEADERS******
******************************************/
/*
    Gets the current timestamp in microseconds.
*/
long usecs();

/*
    Converts microseconds to seconds.
*/
double ms_to_sec(long ms);

/*
    Thread safe random number generator.
*/
int get_rand(int thread_id);

/*
    Given number of each type of job, an estimate is returned on how long 
    a sequential run would have taken.
*/
unsigned long estimate_sequential_timing(int num_small, int num_mid, int num_large);
/*****************************************
************UTILS OPERATIONS HEADERS******
******************************************/

/*****************************************
*************JOBS OPERATIONS HEADERS******
******************************************/
/*
    Job being ran by workers. Runtime depends on the
    passed parameter. Expected parameters are the
    ones specified by job types macros above.
*/
void sample_job(int job_scale);
/*****************************************
*************JOBS OPERATIONS HEADERS******
******************************************/


/*****************************************
*************WORKER NODE HEADERS**********
******************************************/
/*
    Worker thread function.
*/
void * worker_node(void * params);
/*****************************************
*************WORKER NODE HEADERS**********
******************************************/


/*****************************************
*************MASTER NODE HEADERS**********
******************************************/
/*
    Lanuches master node and thus begins
    simulation.
*/
void launch_master_node(int num_workers, int node_selection_strategy, 
    int job_assignment_strategy, int job_type);
/*****************************************
*************MASTER NODE HEADERS**********
******************************************/
#endif