#include "job_simulation.h"

int select_worker_node(WorkerParams * worker_params, int num_workers, int previous_selection, int selection_strategy) {
    if(selection_strategy == NODE_SELECT_SEQUENTIAL) {
        return (previous_selection == num_workers - 1 ? 0 : previous_selection + 1);
    } else if(selection_strategy == NODE_SELECT_RANDOM) {
        // while 'rand' is not thread safe, this function is called by master node only hence
        // there is no cross thread worry
        return rand() % num_workers;
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
    Log * log = (Log *)malloc(sizeof(Log));
    log->log_msg = NULL;
    pthread_mutex_init(&log->log_lock, NULL);
    
    Jobs * jobs = (Jobs *)malloc(sizeof(Jobs));
    jobs->max_capacity = MAX_WORKER_QUEUE_CAPACITY;
    jobs->size = 0;
    jobs->terminate = FALSE;
    jobs->next_job = NULL;
    jobs->last_job = NULL;
    pthread_mutex_init(&worker_param->jobs_lock, NULL);
    pthread_cond_init(&worker_param->work_added, NULL);

    worker_param->jobs = jobs;
    worker_param->log = log;
    worker_param->thread_id = thread_id;
}

/*
    The idea will be to in a retry loop first select node to give job, then try to add, if fail
    repeat this cycle until success
*/
void launch_master_node(int num_workers) {
    pthread_t * worker = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
    WorkerParams * worker_params = (WorkerParams *)malloc(sizeof(WorkerParams) * num_workers);
    printf("Master about to launch slaves.\n"); 
    int i;
    for(i = 0; i < num_workers; i++) {
        init_worker_param(&worker_params[i], i);
        pthread_create(&worker[i], NULL, &worker_node, (void *)&worker_params[i]);
    }

    int n = 0;
    printf("Sequential selection test\n");
    for(i = 0; i < 5; i++) {
        n = select_worker_node(worker_params, num_workers, n, NODE_SELECT_SEQUENTIAL);
        printf("sequential: %i\n", n);
    }

    printf("Random selection test\n");
    for(i = 0; i < 5; i++) {
        n = select_worker_node(worker_params, num_workers, 0, NODE_SELECT_RANDOM);
        printf("random: %i\n", n);
    }


    for(i = 0; i < num_workers; i++) {
        printf("adding 2 jobs to id %i\n", worker_params[i].thread_id);
        pthread_mutex_lock(&worker_params[i].jobs_lock);
        Jobs * jobs = worker_params[i].jobs;
        JobFunction job_functions[3];
        job_functions[0] = &dummy_job;
        job_functions[1] = &dummy_job;
        job_functions[2] = &dummy_job;
        int jobs_added = add_jobs(jobs, job_functions, 3);
        if(jobs_added == TRUE) {
            printf("Job added to node %i, queue_size: %i\n", worker_params[i].thread_id, jobs->size);
        } else {
            printf("Job NOT added to node %i, queue_size: %i\n", worker_params[i].thread_id, jobs->size);
        }
        jobs->terminate = TRUE;
        pthread_cond_broadcast(&worker_params[i].work_added);
        pthread_mutex_unlock(&worker_params[i].jobs_lock);
    }
    

    printf("Master joining on workers.\n");
    printf("SQ selection test\n");
    for(i = 0; i < 5; i++) {
        n = select_worker_node(worker_params, num_workers, 0, NODE_SELECT_SHORTEST_QUEUE);
        printf("SQ: %i\n", n);
    }
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