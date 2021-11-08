#include<fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "commons.h"
#include <errno.h>

// socat pty,raw,echo=0,link=/home/max/Documents/workDev/RT/Task1/ttyS20 pty,raw,echo=0,link=/home/max/Documents/workDev/RT/Task1/ttyS21
// ./reader ~/Documents/workDev/RT/Task1/ttyS20 ~/Documents/workDev/RT/Task1/wyrm.jpg

int main(int argc, char* argv[]) {
    char* path_to_read = get_or_read_parameters(1, &argc, argv, "Enter a path to uart (or Q - for exit):\n");       // get path to file to read chuncks
    char* path_to_save = get_or_read_parameters(2, &argc, argv, "Enter a path to save data (or Q - for exit):\n");  // get path to file to save erceived data
    
    int file_to_read = open(path_to_read, O_RDONLY);
    if (file_to_read < 0) {
        printf("Failed to open file %s, code %d\n", path_to_read, errno);   
        return 0;
    }


    unsigned char* buffer = calloc(CHUNCK_BUFFER, sizeof(char));
    unsigned int total = 0;
    int pages = 1;
    int loop = 1;
    while (loop) {
        struct Chunck chunck = {0};
        if (read(file_to_read, &chunck, sizeof(struct Chunck)) > 0) {
            printf("Received chunk idx %u, size %u\n", chunck.chunck_idx, chunck.size);
            int start_idx = chunck.chunck_idx * CHUNCK_BUFFER;
            int end_idx = start_idx + chunck.size;
            
            // check if we can insert a chunck to our buffer. Enlarge it if it is too small
            if (end_idx >= (pages * CHUNCK_BUFFER)) {
                int old_size = pages * CHUNCK_BUFFER;
                pages = 1 + end_idx / CHUNCK_BUFFER;
                
                buffer = realloc(buffer, pages * CHUNCK_BUFFER * sizeof(char));
                
                // unsigned char* tmp = buffer;
                // buffer = calloc(pages * CHUNCK_BUFFER, sizeof(char));
                // memcpy(buffer, tmp, old_size);
                // free(tmp);
            }
            memcpy(&buffer[start_idx], chunck.payload, chunck.size);
            total += chunck.size;
            
            // if chunck is empty or too small - we got an end of transmission
            if (chunck.size == 0 || chunck.size < CHUNCK_BUFFER) {
                loop = 0;
            }
        }
    }
    close(file_to_read);
    free(path_to_read);
    
    // save data from buffer into file and free allocated datas
    FILE* file_to_save = fopen(path_to_save, "wb+");
    if (file_to_save == NULL) {
        printf("Failed to open file %s, code %d\n", path_to_save, errno);
        return 0;
    }
    if (fwrite(buffer, total, 1, file_to_save) < 0) {
        printf("Failed to write data to file %s\n", path_to_save);
    }
    fclose(file_to_save);
    free(path_to_save);
    
    return 0;
}
