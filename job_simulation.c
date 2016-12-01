#include "job_simulation.h"

void print_selected_parameters_message(int node_selection_strategy, int job_assignment_strategy) {
    printf("====================================\n");
    printf("Number of jobs to distribute: %i\n", NUM_JOBS_TO_DISTRIBUTE);
    printf("Worker queue capacity: %i\n", MAX_WORKER_QUEUE_CAPACITY);
    printf("Worker node selection strategy: ");
    if(node_selection_strategy == NODE_SELECT_SEQUENTIAL) {
        printf("SEQUENTIAL");
    } else if (node_selection_strategy == NODE_SELECT_RANDOM) {
        printf("RANDOM");
    } else if (node_selection_strategy == NODE_SELECT_SHORTEST_QUEUE) {
        printf("SHORTEST QUEUE");
    } else {
        printf("ERROR");
    }
    printf("\n");
    printf("Job distribution strategy: ");
    if(job_assignment_strategy == CONSTANT_JOB_ASSIGNMENT_STRATEGY) {
        printf("CONSTANT, K = %i ", CONSTANT_JOB_CHUNK_SIZE);
        if(CONSTANT_JOB_CHUNK_SIZE > MAX_WORKER_QUEUE_CAPACITY) {
            printf("[WARNING!! Constant chunk exceeds worker queue capacity.");
        }
    } else if (job_assignment_strategy == LINEAR_JOB_ASSIGNMENT_STRATEGY) {
        printf("LINEAR");
    } else if (job_assignment_strategy == EXPONENTIAL_JOB_ASSIGNMENT_STRATEGY) {
        printf("EXPONENTIAL");
    } else {
        printf("ERROR");
    }
    printf("\n====================================\n\n");
}

int main(int argc, char ** argv) {
    int num_workers = 3;
    int node_selection_strategy = NODE_SELECT_SEQUENTIAL;
    int job_assignment_strategy = LINEAR_JOB_ASSIGNMENT_STRATEGY;

    print_selected_parameters_message(node_selection_strategy, job_assignment_strategy);


    launch_master_node(num_workers, node_selection_strategy, job_assignment_strategy);
    /*int i;
    int k = 0;
    long start = usecs();
    for(i = 0; i < INT_MAX; i++) {
        k += 1;
    }
    long end = usecs();
    long diff = end - start;
    printf("took %li ms = %lf sec\n", diff, ms_to_sec(diff));*/
}
