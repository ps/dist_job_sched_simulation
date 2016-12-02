#include "job_simulation.h"

void print_log(Log * log, int thread_id) {
    LogNode * log_node = log->log_msg;
    unsigned long start = 0, end = 0, diff = 1;
    while(log_node != NULL) {
        int msg_id = log_node->msg_id;
        int data = log_node->data;
        unsigned long timestamp = log_node->timestamp;
        if(msg_id == START_PROCESSING_MSG) {
            start = timestamp;
        } else if(msg_id == END_PROCESSING_MSG) {
            end = timestamp;
        }

        printf("msg: %i timestamp: %lu data: %i\n", msg_id, timestamp, data);
        log_node = log_node->next;
    }
    diff = end - start;
    printf("Node %i processed for %li ms = %lf sec\n", thread_id, diff, ms_to_sec(diff));
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
    LogNode * log_node = (LogNode *)malloc(sizeof(LogNode));
    log_node->timestamp = usecs();
    log_node->msg_id = msg_id;
    log_node->data = data;

    pthread_mutex_lock(&log->log_lock);
    log_node->next = log->log_msg;
    log->log_msg = log_node;
    pthread_mutex_unlock(&log->log_lock);
}