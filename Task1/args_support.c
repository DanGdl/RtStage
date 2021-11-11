
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INPUT_PAGE_SIZE 10


int is_exit(char* symbol) {
    return *symbol == 'Q' || *symbol == 'q';
}

// read stuff from stdin till new line or Q/q (exit)
char* get_or_read_parameters(int position, int* argc, char* argv[], const char* message) {
    char* parameter = NULL;
    if (*argc > position) {
        const int size = strlen(argv[position]);
        parameter = calloc(size + 1, sizeof(char));
        memcpy(parameter, argv[position], size);
    } else {
        printf("%s", message);
        parameter = calloc(INPUT_PAGE_SIZE, sizeof(char));
        int pages = 1;
        int idx = 0;
        while(1) {
            char symbol;
            scanf("%c", &symbol);
            int exit = is_exit(&symbol);
            if (symbol == '\n' || exit) {
                if (exit) {
                    parameter[idx] = symbol;
                }
                break;
            }
            parameter[idx] = symbol;
            idx++;
            if (idx >= (pages * INPUT_PAGE_SIZE)) {
                pages++;
                parameter = realloc(parameter, pages * INPUT_PAGE_SIZE * sizeof(char));
                // char* tmp = parameter;
                // parameter = calloc(pages * INPUT_PAGE_SIZE, sizeof(char));
                // memcpy(parameter, tmp, idx);
                // free(tmp);
            }
        }
    }
    return parameter;
} 
