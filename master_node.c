#include "job_simulation.h"

#define DISPLAY_DEBUG 1

int select_worker_node(WorkerParams * worker_params, int num_workers, int previous_selection, int selection_strategy, int master_thread_id) {
    if(selection_strategy == NODE_SELECT_SEQUENTIAL) {
        if(previous_selection < 0) {
            previous_selection = 0;
        } else if (previous_selection >= num_workers) {
            previous_selection = num_workers - 1;
        }
        return (previous_selection == num_workers - 1 ? 0 : previous_selection + 1);
    } else if(selection_strategy == NODE_SELECT_RANDOM) {
        return get_rand(master_thread_id) % num_workers;
    } else if (selection_strategy == NODE_SELECT_SHORTEST_QUEUE) {
        int shortest_queue_size = INT_MAX;
        int node_index = 0;
        int i;
        for(i = 0; i < num_workers; i++) {
            pthread_mutex_lock(&worker_params[i].jobs_lock);
            int worker_queue_size = worker_params[i].jobs->size;
            if(worker_queue_size < shortest_queue_size) {
                shortest_queue_size = worker_queue_size;
                node_index = i;
            }
            pthread_mutex_unlock(&worker_params[i].jobs_lock);
        }
        return node_index;
    }
    printf("UNEXPECTED ERROR: Passed selection strategy was incorrect.");
    return -1;
}

void init_worker_param(WorkerParams * worker_param, int thread_id) {
    worker_param->jobs = init_jobs(worker_param);
    worker_param->log = init_log();
    worker_param->thread_id = thread_id;
}

int adjust_chunk_to_boundry(int chunk_size) {
    if(chunk_size <= 0) {
        return 1;
    } else if(chunk_size > MAX_JOBS_ASSIGNMENT) {
        return MAX_JOBS_ASSIGNMENT;
    }
    return chunk_size;
}

int get_job_distribution_chunk(int previous_chunk_size, int previous_distribution_succeeded, int strategy) {
    if(strategy == CONSTANT_JOB_ASSIGNMENT_STRATEGY) {
        return CONSTANT_JOB_CHUNK_SIZE;
    } else if (strategy == LINEAR_JOB_ASSIGNMENT_STRATEGY) {
        if(previous_distribution_succeeded) {
            previous_chunk_size++;
        } else {
            previous_chunk_size--;
        }
        
        return adjust_chunk_to_boundry(previous_chunk_size);
    } else if (strategy == EXPONENTIAL_JOB_ASSIGNMENT_STRATEGY) {
        if(previous_distribution_succeeded) {
            previous_chunk_size = previous_chunk_size * 2;
        } else {
            previous_chunk_size = previous_chunk_size / 2;
        }

        return adjust_chunk_to_boundry(previous_chunk_size);
    }
    printf("UNEXPECTED ERROR: Passed job assignment strategy was incorrect.");
    return -1;
}

void imitate_network_delay(int master_thread_id) {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = get_rand(master_thread_id) % 50000000;

    nanosleep(&ts, NULL);
}

void terminate_workers(int num_workers, WorkerParams * worker_params) {
    int i;
    for(i = 0; i < num_workers; i++) {
        pthread_mutex_lock(&worker_params[i].jobs_lock);
        Jobs * jobs = worker_params[i].jobs;
        jobs->terminate = TRUE;
        pthread_cond_broadcast(&worker_params[i].work_added);
        pthread_mutex_unlock(&worker_params[i].jobs_lock);
    }
}

void print_and_free_log(Log * log, int thread_id, unsigned long relative_start_timestamp) {
    printf("\nLog for thread id %i\n", thread_id);
    print_log(log, thread_id, relative_start_timestamp);
    free_log(log);
}

void print_worker_logs(int num_workers, WorkerParams * worker_params, unsigned long relative_start_timestamp) {
    int i;
    for(i = 0; i < num_workers; i++) {
        print_and_free_log(worker_params[i].log, i, relative_start_timestamp);
        free(worker_params[i].jobs);
    }
}

void join_workers(int num_workers, pthread_t * worker) {
    int i;
    for(i = 0; i < num_workers; i++) {
        pthread_join(worker[i], NULL);
    }
}

void launch_workers(int num_workers, WorkerParams * worker_params, pthread_t * worker) {
    int i;
    for(i = 0; i < num_workers; i++) {
        init_worker_param(&worker_params[i], i);
        pthread_create(&worker[i], NULL, &worker_node, (void *)&worker_params[i]);
    }
}

void init_master_data(int num_workers, 
    int * master_thread_id, pthread_t ** worker, WorkerParams ** worker_params, Log ** master_log) {

    // Note: since worker threads are indexed 0 through n, use -1 for master thread
    *master_thread_id = MASTER_THREAD_ID;
    *worker = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
    *worker_params = (WorkerParams *)malloc(sizeof(WorkerParams) * num_workers);
    *master_log = init_log();
}

/*
    The idea will be to in a retry loop first select node to give job, then try to add, if fail
    repeat this cycle until success
*/
void launch_master_node(int num_workers, int node_selection_strategy, int job_assignment_strategy, int job_type) {
    
    int master_thread_id;
    pthread_t * worker;
    WorkerParams * worker_params;
    Log * master_log;
    init_master_data(num_workers, &master_thread_id, &worker, &worker_params, &master_log);

    printf("Master about to launch slaves.\n"); 
    launch_workers(num_workers, worker_params, worker);

    printf("Master ready to begin job assignment.\n");
    unsigned long relative_start_timestamp = usecs();

    log_message(master_log, START_PROCESSING_MSG, NO_DATA);
    int jobs_remaining = NUM_JOBS_TO_DISTRIBUTE;
    int node_index = 0;
    // set to max int as job chunk size will end up being bounded to max worker queue size
    int job_chunk_size = INT_MAX;
    int job_distribution_succeeded = FALSE;
    // variable to define what a cycle means, in sequential terms that means going through all nodes,
    // in random terms or shortest queue first terms I decided to define it as attempting as many
    // tries as there are workers available
    int cycle_length = num_workers;

    int count = 0;
    while(jobs_remaining > 0) {
        count++;
        log_message(master_log, JOBS_REMAINING_MSG, jobs_remaining);

        job_chunk_size = get_job_distribution_chunk(job_chunk_size, job_distribution_succeeded, job_assignment_strategy);

        // reset distribution success
        job_distribution_succeeded = FALSE;
        if(job_chunk_size > jobs_remaining) {
            job_chunk_size = jobs_remaining;
        }

        if(DISPLAY_DEBUG && count % 10 == 0) {
            printf("Number of jobs remaining: %i, chunk: %i\n", jobs_remaining, job_chunk_size);
        }

        log_message(master_log, JOB_ASSIGNMENT_RATE_MSG, job_chunk_size);

        JobData * jobs_to_give = generate_job_nodes(job_chunk_size, job_type, master_thread_id);
        
        int iteration = 0;
        while(job_distribution_succeeded == FALSE && iteration < cycle_length) {

            imitate_network_delay(master_thread_id);

            node_index = select_worker_node(worker_params, num_workers, node_index, node_selection_strategy, master_thread_id);

            pthread_mutex_lock(&worker_params[node_index].jobs_lock);
            Jobs * jobs = worker_params[node_index].jobs;

            job_distribution_succeeded = add_jobs(jobs, jobs_to_give, job_chunk_size);
            /*if(job_distribution_succeeded == TRUE) {
                printf("Job added to node %i, queue_size: %i\n", node_index, jobs->size);
            } else {
                printf("Job NOT added to node %i, queue_size: %i\n", node_index, jobs->size);
            }*/
            pthread_cond_broadcast(&worker_params[node_index].work_added);
            pthread_mutex_unlock(&worker_params[node_index].jobs_lock);

            iteration++;
        }

        // since job distribution failed, might as well give up CPU for others
        if(job_distribution_succeeded == FALSE) {
            sched_yield();
        }

        if(job_distribution_succeeded == TRUE) {
            jobs_remaining = jobs_remaining - job_chunk_size;
        }
        free(jobs_to_give);
    }

    terminate_workers(num_workers, worker_params);

    log_message(master_log, END_PROCESSING_MSG, NO_DATA);
    
    // joins threads
    printf("Master joining on workers.\n");
    join_workers(num_workers, worker);
    unsigned long end_timestamp = usecs();
    unsigned long threaded_timing = end_timestamp - relative_start_timestamp;

    // joins threads, prints log, cleans up worker allocated data
    print_worker_logs(num_workers, worker_params, relative_start_timestamp);

    // prints master log
    print_and_free_log(master_log, master_thread_id, relative_start_timestamp);

    int num_small = get_job_frequency(SMALL_JOB);
    int num_mid = get_job_frequency(MID_JOB);
    int num_large = get_job_frequency(LARGE_JOB);
    printf("\n\nNumber of LARGE jobs: %i\n", num_large);
    printf("Number of MID jobs: %i\n", num_mid);
    printf("Number of SMALL jobs: %i\n", num_small);

    unsigned long sequential_estimation_timing = estimate_sequential_timing(num_small, num_mid, num_large);

    printf("Threaded timing: %li ms = %lf sec \n", threaded_timing, ms_to_sec(threaded_timing));
    printf("Estimated sequential processing time: %li ms = %lf sec\n", sequential_estimation_timing, ms_to_sec(sequential_estimation_timing));
    printf("Rough speedup: %.2fx\n", sequential_estimation_timing / (double) threaded_timing);
    
    free(worker);
    free(worker_params);
}