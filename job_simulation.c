#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stddef.h> // for NULL
#include <sys/time.h>

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
    pthread_mutex_t queue_lock;
    pthread_cond_t work_added;
} WorkerParams;


void * worker_node(void * params);
void launch_master_node();
long usecs();


void print_log(Log * log) {
    LogNode * log_node = log->log_msg;
    while(log_node != NULL) {
        printf("msg: %i timestamp: %lu\n", log_node->msg_id, log_node->timestamp);
        log_node = log_node->next;
    }
}

void free_log(Log * log) {
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


long usecs() {
    struct timeval t;
    gettimeofday(&t,NULL);
    return t.tv_sec*1000000+t.tv_usec;
}

void * worker_node(void * params) {
    WorkerParams * my_params = (WorkerParams *) params;
    int thread_id = my_params->thread_id;
    Log * log = my_params->log;

    printf("Node id %i reporting.\n", thread_id);
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

    worker_param->log = log;
    worker_param->thread_id = thread_id;
    pthread_mutex_init(&worker_param->queue_lock, NULL);
    pthread_cond_init(&worker_param->work_added, NULL);
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
    int num_workers = 3;
    launch_master_node(num_workers);
}
