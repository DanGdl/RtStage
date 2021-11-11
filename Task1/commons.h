#ifndef TASK_1_H_
#define TASK_1_H_

#include "queue.h"
#include <pthread.h>

#define CHUNCK_BUFFER 100


typedef struct ThreadParams {
	pthread_mutex_t* mutex;
    int* write_to;
    Queue_t* queue;
} ThreadParams_t;

typedef struct FileParams {
    int file_descriptor;
    long file_id;
    unsigned long chunck_counter;
	pthread_mutex_t* mutex;
} FileParams_t;

typedef struct Chunck {
    unsigned long chunck_idx;
    unsigned int size;
    long file_id;
    unsigned char payload[CHUNCK_BUFFER];
} Chunck_t;


typedef struct CacheItem {
    unsigned char* buffer;
    unsigned int total;
    unsigned int pages;
    long file_id;
} CacheItem_t;

int is_exit(char* symbol);

char* get_or_read_parameters(int position, int* argc, char* argv[], const char* message);

#endif 
