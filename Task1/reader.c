#include<fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "commons.h"
#include <errno.h>
#include <limits.h>

// socat pty,raw,echo=0,link=/home/max/Documents/workDev/RtStage/Task1/ttyS20 pty,raw,echo=0,link=/home/max/Documents/workDev/RtStage/Task1/ttyS21
// ./reader ~/Documents/workDev/RtStage/Task1/ttyS21

#define CACHE_SIZE 50
#define NO_POSITION -1

// save data from buffer into file and free allocated datas
void save_to_file(CacheItem_t* item, char* cwd) {
    char path_to_save[PATH_MAX];
    sprintf(path_to_save, "%s/%ld", cwd, item -> file_id);

    FILE* file_to_save = fopen(path_to_save, "wb+");
    if (file_to_save == NULL) {
        printf("Failed to open file %s, code %d\n", path_to_save, errno);
        return;
    }
    if (fwrite(item -> buffer, item -> total, 1, file_to_save) < 0) {
        printf("Failed to write data to file %s\n", path_to_save);
    }
    fclose(file_to_save);
    printf("Saved to file %s\n", path_to_save);
}


int main(int argc, char* argv[]) {
    char* path_to_read = get_or_read_parameters(1, &argc, argv, "Enter a path to uart (or Q - for exit):\n");       // get path to file to read chuncks
    int file_to_read = open(path_to_read, O_RDONLY);
    if (file_to_read < 0) {
        printf("Failed to open file %s, code %d\n", path_to_read, errno);
        return 0;
    }

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    } else {
        perror("getcwd() error");
    }

    int free_idx = 0;
    long indexes[CACHE_SIZE] = {};
    for (int i = 0; i < CACHE_SIZE; i++) {
    	indexes[i] = NO_POSITION;
    }
    CacheItem_t** buffers = calloc(CACHE_SIZE, sizeof(CacheItem_t*));

    while (1) {
        struct Chunck chunck = {0};
        if (read(file_to_read, &chunck, sizeof(Chunck_t)) > 0) {
            // printf("Received chunk fId %ld idx %u, size %u\n", chunck.file_id, chunck.chunck_idx, chunck.size);
            int idx = NO_POSITION;
            for (int i = 0; i < CACHE_SIZE; i++) {
                if (indexes[i] == chunck.file_id) {
                    idx = i;
                    break;
                }
            }
            if (idx == NO_POSITION) {
                CacheItem_t* item = calloc(1, sizeof(CacheItem_t));
                item -> total = 0;
                item -> pages = 1;
                item -> file_id = chunck.file_id;
                item -> buffer = calloc(CHUNCK_BUFFER, sizeof(char));

                printf("Received fId %ld, cache pos %d\n", chunck.file_id, free_idx);
                buffers[free_idx] = item;
                indexes[free_idx] = chunck.file_id;
                idx = free_idx;

                for (int i = 0; i < CACHE_SIZE; i++) {
                    if (indexes[i] == NO_POSITION) {
                        free_idx = i;
                        break;
                    }
                }
                // cache can be full!!
            }
            CacheItem_t* item = buffers[idx];

            unsigned long start_idx = chunck.chunck_idx * CHUNCK_BUFFER;
            unsigned long end_idx = start_idx + chunck.size;

            // check if we can insert a chunck to our buffer. Enlarge it if it is too small
            if (end_idx >= ((item -> pages) * CHUNCK_BUFFER)) {
                item -> pages = 1 + end_idx / CHUNCK_BUFFER;
                item -> buffer = realloc(item -> buffer, (item -> pages) * CHUNCK_BUFFER * sizeof(char));
            }
            memcpy(&(item -> buffer[start_idx]), chunck.payload, chunck.size);
            item -> total += chunck.size;

            // if chunck is empty or too small - we got an end of transmission
            if (chunck.size == 0 || chunck.size < CHUNCK_BUFFER) {
            	printf("Received last chunk fId %ld idx %u, size %u\n", chunck.file_id, chunck.chunck_idx, chunck.size);
                save_to_file(item, cwd);
                buffers[idx] = NULL;
                indexes[idx] = NO_POSITION;
                free(item -> buffer);
                free(item);

                for (int i = 0; i < CACHE_SIZE; i++) {
                    if (indexes[i] == NO_POSITION) {
                        free_idx = i;
                        break;
                    }
                }
            }
        }
    }
    close(file_to_read);
    free(path_to_read);
    return 0;
}
