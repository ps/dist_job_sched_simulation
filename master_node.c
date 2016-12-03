#include "job_simulation.h"

int select_worker_node(WorkerParams * worker_params, int num_workers, int previous_selection, int selection_strategy, int master_thread_id) {
    if(selection_strategy == NODE_SELECT_SEQUENTIAL) {
        if(previous_selection < 0) {
            previous_selection = 0;
        } else if (previous_selection >= num_workers) {
            previous_selection = num_workers - 1;
        }
        return (previous_selection == num_workers - 1 ? 0 : previous_selection + 1);
    } else if(selection_strategy == NODE_SELECT_RANDOM) {
        // since
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

Log * init_log() {
    Log * log = (Log *)malloc(sizeof(Log));
    log->log_msg = NULL;
    log->first_log_timestamp = 0;
    pthread_mutex_init(&log->log_lock, NULL);
    return log;
}

Jobs * init_jobs(WorkerParams * worker_param) {
    Jobs * jobs = (Jobs *)malloc(sizeof(Jobs));
    jobs->max_capacity = MAX_WORKER_QUEUE_CAPACITY;
    jobs->size = 0;
    jobs->terminate = FALSE;
    jobs->next_job = NULL;
    jobs->last_job = NULL;
    pthread_mutex_init(&worker_param->jobs_lock, NULL);
    pthread_cond_init(&worker_param->work_added, NULL);
    return jobs;
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
    ts.tv_nsec = get_rand(master_thread_id) % 250000000;

    nanosleep(&ts, NULL);
}

/*
    The idea will be to in a retry loop first select node to give job, then try to add, if fail
    repeat this cycle until success
*/
void launch_master_node(int num_workers, int node_selection_strategy, int job_assignment_strategy, int job_type) {
    // Note: since worker threads are indexed 0 through n, use -1 for master thread
    int master_thread_id = -1;
    pthread_t * worker = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
    WorkerParams * worker_params = (WorkerParams *)malloc(sizeof(WorkerParams) * num_workers);
    Log * master_log = init_log();
    printf("Master about to launch slaves.\n"); 
    int i;
    for(i = 0; i < num_workers; i++) {
        init_worker_param(&worker_params[i], i);
        pthread_create(&worker[i], NULL, &worker_node, (void *)&worker_params[i]);
    }

    printf("Master waiting 1 second to give other threads chance to be spawned and scheduled.\n");
    sleep(1);
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
    int first = TRUE;

    while(jobs_remaining > 0) {
        log_message(master_log, JOBS_REMAINING_MSG, jobs_remaining);

        //printf("Jobs remaining: %i\n", jobs_remaining);
        job_chunk_size = get_job_distribution_chunk(job_chunk_size, job_distribution_succeeded, job_assignment_strategy);
        if(first) {
            printf("first: %i\n", job_chunk_size);
            first = FALSE;
        }
        // reset distribution success
        job_distribution_succeeded = FALSE;
        if(job_chunk_size > jobs_remaining) {
            job_chunk_size = jobs_remaining;
        }

        log_message(master_log, JOB_ASSIGNMENT_RATE_MSG, job_chunk_size);

        JobData * jobs_to_give = generate_job_nodes(job_chunk_size, job_type, master_thread_id);
        
        imitate_network_delay(master_thread_id);

        int iteration = 0;
        while(job_distribution_succeeded == FALSE && iteration < cycle_length) {
            node_index = select_worker_node(worker_params, num_workers, node_index, node_selection_strategy, master_thread_id);

            //printf("Attempting to add %i jobs to node %i \n", job_chunk_size, node_index);
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

    // terminate all workers
    for(i = 0; i < num_workers; i++) {
        pthread_mutex_lock(&worker_params[i].jobs_lock);
        Jobs * jobs = worker_params[i].jobs;
        jobs->terminate = TRUE;
        pthread_cond_broadcast(&worker_params[i].work_added);
        pthread_mutex_unlock(&worker_params[i].jobs_lock);
    }
    log_message(master_log, END_PROCESSING_MSG, NO_DATA);
    

    printf("Master joining on workers.\n");
    for(i = 0; i < num_workers; i++) {
        pthread_join(worker[i], NULL);
        printf("\nPrinting log for thread id %i\n", i);
        print_log(worker_params[i].log, i, FALSE, FALSE, relative_start_timestamp);
        free(worker_params[i].jobs);
        free_log(worker_params[i].log);
    }
    printf("\nPrinting log for thread id %i (master)\n", master_thread_id);
    print_log(master_log, master_thread_id, FALSE, TRUE, relative_start_timestamp);
    free_log(master_log);

    printf("Number of LARGE jobs: %i\n", get_job_frequency(LARGE_JOB));
    printf("Number of MID jobs: %i\n", get_job_frequency(MID_JOB));
    printf("Number of SMALL jobs: %i\n", get_job_frequency(SMALL_JOB));
    
    free(worker);
    free(worker_params);
    printf("Master exiting.\n");
}