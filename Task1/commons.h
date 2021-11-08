#ifndef TASK_1_H_
#define TASK_1_H_

#define CHUNCK_BUFFER 92

// TODO: prepare to send  a list of files
struct ThreadParams {
    unsigned int chunck_counter;
	pthread_mutex_t* mutex;
    int* read_from;
    int* write_to;
};

// TODO: prepare to send  a list of files
struct Chunck {
    unsigned int chunck_idx;
    unsigned byte size;
    unsigned char payload[CHUNCK_BUFFER];
};

char* get_or_read_parameters(int position, int* argc, char* argv[], const char* message);

#endif 
