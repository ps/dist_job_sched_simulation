#include "job_simulation.h"

void print_log(Log * log, int thread_id, int stdout, int master, unsigned long relative_start) {
    char str[500];
    LogNode * log_node = log->log_msg;
    unsigned long start = 0, end = 0, diff = 1;
    FILE * fp;
    printf("Remember that logs are printed from last event to first event.\n");
    if(!stdout) {
        sprintf(str, "thread%i.dat", thread_id);
        fp = fopen(str, "ab+");
    }
    while(log_node != NULL) {
        int msg_id = log_node->msg_id;
        int data = log_node->data;
        unsigned long timestamp = log_node->timestamp;
        // fetch node execution time
        if(msg_id == START_PROCESSING_MSG) {
            start = timestamp;
        } else if(msg_id == END_PROCESSING_MSG) {
            end = timestamp;
        }

        // do something about WORKER_SLEEP_TIME_MSG here such as add up all the time


        if(stdout) {
            printf("msg: %i timestamp: %lu data: %i\n", msg_id, timestamp, data);
        } else {
            // (real time, adjusted_time, data, message id)
            // separated via the if statements just in case the log messeges accidentally make into the improper log
            if(master && msg_id == JOB_ASSIGNMENT_RATE_MSG) {
                fprintf(fp, "%li, %li, %i, %i\n", timestamp, timestamp - relative_start, data, JOB_ASSIGNMENT_RATE_MSG);
            } else if(master && msg_id == JOBS_REMAINING_MSG) {
                fprintf(fp, "%li, %li, %i, %i\n", timestamp, timestamp - relative_start, data, JOBS_REMAINING_MSG);
            } else if (!master && msg_id == WORKER_QUEUE_SIZE_MSG) {
                fprintf(fp, "%li, %li, %i, %i\n", timestamp, timestamp - relative_start, data, WORKER_QUEUE_SIZE_MSG);
            }
        }

        log_node = log_node->next;
    }
    diff = end - start;
    printf("Node %i processed for %li ms = %lf sec\n", thread_id, diff, ms_to_sec(diff));
    if(!stdout) {
        fclose(fp);
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

void log_message(Log * log, int msg_id, int data) {
    unsigned long timestamp = usecs();
    if(log->first_log_timestamp == 0) {
        log->first_log_timestamp = timestamp;
    }
    log->last_log_timestamp = timestamp;
    LogNode * log_node = (LogNode *)malloc(sizeof(LogNode));
    log_node->timestamp = timestamp;
    log_node->msg_id = msg_id;
    log_node->data = data;

    pthread_mutex_lock(&log->log_lock);
    log_node->next = log->log_msg;
    log->log_msg = log_node;
    pthread_mutex_unlock(&log->log_lock);
}