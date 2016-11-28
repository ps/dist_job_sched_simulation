#include "job_simulation.h"

void print_log(Log * log) {
    LogNode * log_node = log->log_msg;
    while(log_node != NULL) {
        printf("msg: %i timestamp: %lu\n", log_node->msg_id, log_node->timestamp);
        log_node = log_node->next;
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

void log_message(Log * log, int msg_id) {
    LogNode * log_node = (LogNode *)malloc(sizeof(LogNode));
    log_node->msg_id = msg_id;
    log_node->timestamp = usecs();

    pthread_mutex_lock(&log->log_lock);
    log_node->next = log->log_msg;
    log->log_msg = log_node;
    pthread_mutex_unlock(&log->log_lock);
}