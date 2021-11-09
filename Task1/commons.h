#ifndef TASK_1_H_
#define TASK_1_H_

#include "queue.h"

#define CHUNCK_BUFFER 100


struct ThreadParams {
	pthread_mutex_t* mutex;
    int* write_to;
    struct Queue_t* files;
};

struct FileParams {
    int file_descriptor;
    unsigned int chunck_counter;
	pthread_mutex_t* mutex;
};

// TODO: prepare to send  a list of files
struct Chunck {
    unsigned int chunck_idx;
    unsigned int size;
    unsigned char payload[CHUNCK_BUFFER];
};

int is_exit(char* symbol);

char* get_or_read_parameters(int position, int* argc, char* argv[], const char* message);

#endif 
