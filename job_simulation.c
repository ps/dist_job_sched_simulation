#include "job_simulation.h"

int NUM_JOBS_TO_DISTRIBUTE;
int JOB_TYPE;
int NUM_WORKERS;
int MAX_WORKER_QUEUE_CAPACITY;
int MAX_JOBS_ASSIGNMENT;
int NODE_SELECTION_STRATEGY;
int JOB_DISTRIBUTION_STRATEGY;
int CONSTANT_JOB_CHUNK_SIZE;

int set_num_jobs(char * num_jobs) {
    printf("Number of jobs to distribute: \t\t\t");
    NUM_JOBS_TO_DISTRIBUTE = atoi(num_jobs);
    if(NUM_JOBS_TO_DISTRIBUTE < 1) {
        printf("Invalid! Must be a non-zero positive integer.\n");
        return FALSE;
    }
    printf("%i\n", NUM_JOBS_TO_DISTRIBUTE);
    return TRUE;
}

int set_job_type(char * job_type_str) {
    int job_type = atoi(job_type_str);
    printf("Type of jobs being ran by workers: \t\t");
    if(job_type == 4) {
        JOB_TYPE = VARIED_JOB;
        printf("VARIED");
    } else if (job_type == 3) {
        JOB_TYPE = LARGE_JOB;
        printf("LARGE");
    } else if (job_type == 2) {
        JOB_TYPE = MID_JOB;
        printf("MID");
    } else if (job_type == 1) {
        JOB_TYPE = SMALL_JOB;
        printf("SMALL");
    } else {
        printf("Incorrect job type specified!\n");
        return FALSE;
    }
    printf(" jobs\n");
    return TRUE;
}

int set_num_workers(char * num_workers_str) {
    printf("Number of workers: \t\t\t\t");
    NUM_WORKERS = atoi(num_workers_str);
    if(NUM_WORKERS < 1 || NUM_WORKERS > 5000) {
        printf("Invalid! Must be a non-zero positive integer less than 5000.\n");
        return FALSE;
    }
    printf("%i\n", NUM_WORKERS);
    return TRUE;
}

int set_worker_queue_cap(char * queue_cap_str) {
    printf("Worker queue capacity: \t\t\t\t");
    MAX_WORKER_QUEUE_CAPACITY = atoi(queue_cap_str);
    if(MAX_WORKER_QUEUE_CAPACITY < 1) {
        printf("Invalid! Must be a non-zero positive integer.\n");
        return FALSE;
    }
    printf("%i\n", MAX_WORKER_QUEUE_CAPACITY);
    return TRUE;
}

int set_max_assign_size(char * max_assign) {
    printf("Max number of jobs assigned by master: \t\t");
    MAX_JOBS_ASSIGNMENT = atoi(max_assign);
    if(MAX_JOBS_ASSIGNMENT < 1) {
        printf("Invalid! Must be a non-zero positive integer.\n");
        return FALSE;
    } else if (MAX_JOBS_ASSIGNMENT > MAX_WORKER_QUEUE_CAPACITY) {
        printf("Invalid! Cannot exceed worker queue capacity.\n");
        return FALSE;
    }
    printf("%i\n", MAX_JOBS_ASSIGNMENT);
    return TRUE;
}

int set_worker_selection_strategy(char * strategy_str) {
    int strategy = atoi(strategy_str);
    printf("Worker node selection strategy: \t\t");
    if(strategy == 1) {
        NODE_SELECTION_STRATEGY = NODE_SELECT_SEQUENTIAL;
        printf("SEQUENTIAL");
    } else if(strategy == 2) {
        NODE_SELECTION_STRATEGY = NODE_SELECT_RANDOM;
        printf("RANDOM");
    } else if(strategy == 3) {
        NODE_SELECTION_STRATEGY = NODE_SELECT_SHORTEST_QUEUE;
        printf("SHORTEST QUEUE");
    } else {
        printf("Incorrect selection strategy!\n");
        return FALSE;
    }
    printf("\n");
    return TRUE;
}

int set_distribution_strategy(char * strategy_str) {
    int strategy = atoi(strategy_str);
    printf("Job distribution strategy: \t\t\t");
    if(strategy == 1) {
        JOB_DISTRIBUTION_STRATEGY = CONSTANT_JOB_ASSIGNMENT_STRATEGY;
        printf("CONSTANT");
    } else if(strategy == 2) {
        JOB_DISTRIBUTION_STRATEGY = LINEAR_JOB_ASSIGNMENT_STRATEGY;
        printf("LINEAR");
    } else if(strategy == 3) {
        JOB_DISTRIBUTION_STRATEGY = EXPONENTIAL_JOB_ASSIGNMENT_STRATEGY;
        printf("EXPONENTIAL");
    } else {
        printf("Incorrect job distribution strategy!\n");
        return FALSE;
    }
    printf("\n");
    return TRUE;
}

int set_constant_distribution_size(char * size_str) {
    printf("Constant chunk distribution size (used only if job distribution is CONSTANT): ");
    CONSTANT_JOB_CHUNK_SIZE = atoi(size_str);
    if(CONSTANT_JOB_CHUNK_SIZE < 1) {
        printf("Invalid! Must be a non-zero positive integer.\n");
        return FALSE;
    } else if (CONSTANT_JOB_CHUNK_SIZE > MAX_WORKER_QUEUE_CAPACITY) {
        printf("Invalid! Cannot exceed the size of worker queue capacity.\n");
        return FALSE;
    }
    printf("%i\n", CONSTANT_JOB_CHUNK_SIZE);
    return TRUE;
}

int read_config(char * filename) {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        return FALSE;
    }

    printf("==============PARAMETERS===========================\n");
    int line_num = 1;
    while ((read = getline(&line, &len, fp)) != -1) {
        if(line_num == 2) {
            if(!set_num_jobs(line)) {
                return FALSE;
            }
        } else if (line_num == 5) {
            if(!set_job_type(line)) {
                return FALSE;
            }
        } else if (line_num == 8) {
            if(!set_num_workers(line)) {
                return FALSE;
            }
        } else if (line_num == 11) {
            if(!set_worker_queue_cap(line)) {
                return FALSE;
            }
        } else if (line_num == 14) {
            if(!set_max_assign_size(line)) {
                return FALSE;
            }
        } else if (line_num == 17) {
            if(!set_worker_selection_strategy(line)) {
                return FALSE;
            }
        } else if (line_num == 20) {
            if(!set_distribution_strategy(line)) {
                return FALSE;
            }
        } else if (line_num == 23) {
            if(!set_constant_distribution_size(line)) {
                return FALSE;
            }
        }

        line_num++;
    }

    if(line_num < 23) {
        printf("Configuration file did not contain all of the necessary configurations.\n");
        return FALSE;
    }

    fclose(fp);
    if (line) {
        free(line);
    }
    printf("==============PARAMETERS===========================\n\n");
    return TRUE;
}

int main(int argc, char ** argv) {
    if(argc != 2) {
        printf("Configuration file was not specified, run with ./job_simulation <conf-file>\n");
        return 1;
    }
    if(!read_config(argv[1])) {
        return 1;
    }

    launch_master_node(NUM_WORKERS, NODE_SELECTION_STRATEGY, JOB_DISTRIBUTION_STRATEGY, JOB_TYPE);

    /*long start = usecs();
    sample_job(SMALL_JOB);
    long end = usecs();
    long diff = end - start;
    printf("SMALL: 1/4 full took %li ms = %lf sec\n", diff, ms_to_sec(diff));


    start = usecs();
    sample_job(MID_JOB);
    end = usecs();
    diff = end - start;
    printf("MID: 1/2 full took %li ms = %lf sec\n", diff, ms_to_sec(diff));

    start = usecs();
    sample_job(LARGE_JOB);
    end = usecs();
    diff = end - start;
    printf("LARGE: 1 full took %li ms = %lf sec\n", diff, ms_to_sec(diff));*/
    return 0;
}
