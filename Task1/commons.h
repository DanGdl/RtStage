#ifndef TASK_1_H_
#define TASK_1_H_

#define INPUT_PAGE_SIZE 10
#define BUFFER_SIZE 92

struct ThreadParams {
    unsigned int chunck_counter;
    int send_chuncks;
	pthread_mutex_t* mutex;
    FILE* read_from;
    int* write_to;
};

struct Chunck {
    unsigned int chunck_idx;
    unsigned int size;
    unsigned char payload[BUFFER_SIZE];
};

char* get_or_read_parameters(int position, int* argc, char* argv[], const char* message);

#endif 
