#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include "commons.h"
#include "queue.h"

// ./writer ~/Documents/workDev/RtStage/Task1/ttyS20 ~/Documents/workDev/RtStage/Task1/ex1.jpg ~/Documents/workDev/RtStage/Task1/ex2.jpg ~/Documents/workDev/RtStage/Task1/ex1.jpg ~/Documents/workDev/RtStage/Task1/ex3.jpg ~/Documents/workDev/RtStage/Task1/ex4.jpg ~/Documents/workDev/RtStage/Task1/ex5.jpg ~/Documents/workDev/RtStage/Task1/ex6.jpg

//  /home/max/Documents/workDev/RtStage/Task1/ex1.jpg /home/max/Documents/workDev/RtStage/Task1/ex2.jpg /home/max/Documents/workDev/RtStage/Task1/ex3.jpg /home/max/Documents/workDev/RtStage/Task1/ex4.jpg /home/max/Documents/ // workDev/RtStage/Task1/ex5.jpg /home/max/Documents/workDev/RtStage/Task1/ex6.jpg

#define NO_FILE -1

size_t send_chunk(const pthread_t* thread_id, FileParams_t* data, int* write_to) {
    Chunck_t chunck = {
        .chunck_idx = data -> chunck_counter,
        .file_id = data -> file_id,
    };
    size_t size = read(data -> file_descriptor, &chunck.payload, CHUNCK_BUFFER);
    if (size == 0) {
        printf("Thread %lu EOF!!\n", *thread_id);
    } else if (size < 0) {
        printf("Thread %lu error!!\n", *thread_id);
    } else {
        data -> chunck_counter += 1;
        chunck.size = (unsigned int) size;

        // printf("Thread %lu fd %d chunk %u read %u bytes\n", *thread_id, data -> file_descriptor, chunck.chunck_idx, chunck.size);
        if (size > 0) {
            size_t chunk_size = sizeof(Chunck_t);
            size_t sent_amount = write(*write_to, &chunck, chunk_size);
            if (sent_amount < chunk_size) {
                printf("Thread %lu fd %d sent only %ld bytes of %ld!!\n", *thread_id, data -> file_descriptor, sent_amount, chunk_size);
            }
        }
    }
    return size;
}

// method, for execution in threads. reads from file a chunks of data and send it to specified file
void* task_writer(void* arg) {
    ThreadParams_t* params = (ThreadParams_t*) arg;
    const pthread_t thread_id = pthread_self();
    while (1) {
         // lock params, access to queue
        if (pthread_mutex_lock(params -> mutex) == 0) {
            if (queue_size(params -> queue) == 0) {
            	pthread_mutex_unlock(params -> mutex);
                break;
            }
            Node_t* node = params -> queue -> head;

            while (node != NULL) {
                // get one from queue and try to lock. if success - send, other way - try to get next one
                FileParams_t* data = (FileParams_t*) node -> data;
                if (pthread_mutex_trylock(data -> mutex) == 0) {
                    pthread_mutex_unlock(params -> mutex);

                    if (send_chunk(&thread_id, data, params -> write_to) <= 0) {
                    	// if we send all data - free memory and remove from queue
                        if (pthread_mutex_lock(params -> mutex) == 0) {
                            queue_remove(params -> queue, node);
                            pthread_mutex_t* mutex = data -> mutex;
                            printf("Thread %lu fd %d sent %ld chunks. Removed from queue\n", thread_id, data -> file_descriptor, data -> chunck_counter);
                            close(data -> file_descriptor);
                            free(data);
                            pthread_mutex_unlock(mutex);
                            pthread_mutex_destroy(mutex);
                            free(mutex);
                            pthread_mutex_unlock(params -> mutex);
                            break;
                        }
                    }
                    pthread_mutex_unlock(data -> mutex);
                    break;
                } else {
                    node = node -> next;
                }
            }
            pthread_mutex_unlock(params -> mutex);
        }
        // TODO: fix later, leads to problem with mutexes RtEx1: ../nptl/pthread_mutex_lock.c:81: __pthread_mutex_lock: Assertion `mutex->__data.__owner == 0' failed.
        sleep(0.1);
    }
    printf("Thread %lu done\n", thread_id);
    return NULL;
}


int main(int argc, char* argv[]) {
    char* path_to_write = get_or_read_parameters(1, &argc, argv, "Enter a path to uart (or Q - for exit):\n");  // get path to file to send chuncks
    int file_to_write = open(path_to_write, O_WRONLY);
    if (file_to_write < 0) {
        printf("Failed to open file %s, code %d\n", path_to_write, file_to_write);
        return 0;
    }

    const int files_count = argc - 2;
    if (files_count <= 0) {
        close(file_to_write);
        free(path_to_write);
        printf("Please enter files to send\n");
        return 0;
    }
    int bad_addresses = 0;
    int* descriptors = calloc(files_count, sizeof(int));
    for (int i = 0; i < files_count; i++) {
        descriptors[i] = open(argv[i + 2], O_RDONLY);
        if (descriptors[i] < 0) {
            printf("Failed to open file %s, code %d\n", argv[i + 2], descriptors[i]);
            descriptors[i] = NO_FILE;
            bad_addresses++;
        }
    }

    if (bad_addresses == files_count) {
        close(file_to_write);
        free(path_to_write);
        printf("Files for sending are not valid\n");
        return 0;
    }

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    srand(time(NULL));
    const int threads_amount = rand() % 10 + 1;
    printf("Threads %d, chunk size %lu\n", threads_amount, sizeof(Chunck_t));
    pthread_t* thread_ids = calloc(threads_amount, sizeof(pthread_t));

    // create parameters and shared data for threads
    ThreadParams_t task_params = {
        .mutex = &mutex,
        .write_to = &file_to_write,
        .queue = queue_create(),
    };

    for (int i = 0; i < files_count; i++) {
        if (descriptors[i] == NO_FILE) {
            continue;
        }
        struct timeval time;
        gettimeofday(&time, NULL);
        long millis = (long)((time.tv_sec) * 1000 + (time.tv_usec) / 1000);

        FileParams_t* file_params = calloc(1, sizeof(FileParams_t));
        file_params -> file_descriptor = descriptors[i];
        file_params -> file_id = millis;
        // TODO: fix later. All files receive same id because all if quick
        sleep(1);

        file_params -> chunck_counter = 0;
        file_params -> mutex = calloc(1, sizeof(pthread_mutex_t));
        pthread_mutex_init(file_params -> mutex, NULL);

        queue_add(task_params.queue, file_params);
    }

    // launch threads
    for (int i = 0; i < threads_amount; i++) {
        pthread_create(&thread_ids[i], NULL, &task_writer, &task_params);
    }
    for (int i = 0; i < threads_amount; ++i) {
        pthread_join(thread_ids[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    free(descriptors);
    free(thread_ids);
    close(file_to_write);
    free(path_to_write);

    return 0;
}
