#include "job_simulation.h"

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