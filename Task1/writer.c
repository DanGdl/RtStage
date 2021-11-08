#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "commons.h"

// ./writer ~/lazy_wirm.jpg ~/Documents/workDev/RT/Task1/ttyS20

// method, for execution in threads. reads from file a chunks of data and send it to specified file
void* task_writer(void* arg) {
    struct ThreadParams* params = (struct ThreadParams*) arg;
    const pthread_t thread_id = pthread_self();
    int loop = 1;
    while (loop) {
		// TODO: prepare to send  a list of files
        if (pthread_mutex_lock(params -> mutex) == 0) {
            struct Chunck chunck = {
                .chunck_idx = params -> chunck_counter,
            };
            size_t read = 0;
			int size = read(*(params -> read_from), &chunck.payload[read], CHUNCK_BUFFER);
			// TODO: handle
			if (size > 0) {
				read += size;
			} else if (feof(params -> read_from)) {
				printf("Thread %lu EOF!!\n", thread_id);
				loop = 0;
			} else if (ferror(params -> read_from)) {
				printf("Thread %lu error reading file\n", thread_id);
				loop = 0;
			}
            
            params -> chunck_counter += 1;
            chunck.size = read;
            pthread_mutex_unlock(params -> mutex);
			
            printf("Thread %lu chunk %u read %u bytes\n", thread_id, chunck.chunck_idx, chunck.size);
            if (read > 0) {
                write(*(params -> write_to), &chunck, sizeof(struct Chunck));
            }
        }
    }
    printf("Thread %lu done\n", thread_id);
    return NULL;
}


int main(int argc, char* argv[]) {
    char* path_to_read = get_or_read_parameters(1, &argc, argv, "Enter a path to file (or Q - for exit):\n");   // get path to file to read chuncks
    char* path_to_write = get_or_read_parameters(2, &argc, argv, "Enter a path to uart (or Q - for exit):\n");  // get path to file to send chuncks
    
    int file_to_read = open(path_to_read, O_RDONLY);
    if (file_to_read < 0) {
        printf("Failed to open file %s, code %d\n", path_to_read, file_to_read);   
        return 0;
    }
    int file_to_write = open(path_to_write, O_WRONLY);
    if (file_to_read < 0) {
        printf("Failed to open file %s, code %d\n", path_to_write, file_to_write);
        return 0;
    }
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    srand(time(NULL));
    const int threads_amount = rand() % 10;
    printf("Threads %d\n", threads_amount);
    pthread_t* thread_ids = calloc(threads_amount, sizeof(pthread_t));
    
	// TODO: get size of file and count amount of chunks needed
    // create parameters and shared data for threads
    struct ThreadParams task_params = {
        .mutex = &mutex,
        .read_from = &file_to_read,
        .write_to = &file_to_write,
        .chunck_counter = 0,
    };
    
    // launch threads and wait to end of work
    for (int i = 0; i < threads_amount; i++) {
        pthread_create(&thread_ids[i], NULL, &task_writer, &task_params);
    }
    for (int i = 0; i < threads_amount; ++i) {
        pthread_join(thread_ids[i], NULL);
    }

    // free allocated memory
    free(thread_ids);

    close(file_to_read);
    close(file_to_write);

    free(path_to_read);
    free(path_to_write);
    
    return 0;
}
