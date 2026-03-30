#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

char *read_file(char *path) {
    char *ret = NULL;
    FILE *f = fopen(path, "r");
    if(!f) goto error;
    if(fseek(f, 0, SEEK_END) == -1) goto error;
    size_t file_size = ftell(f);
    if(fseek(f, 0, SEEK_SET) == -1) goto error;
    if(!(ret = malloc(file_size + 1))) goto error;
    if(fread(ret, 1, file_size, f) != file_size) if(!feof(f)) goto error;
    ret[file_size] = 0;
    return ret;
error:
    perror(NULL);
    if(ret) free(ret);
    if(f) fclose(f);
    return NULL;
}


#define BRAINFUCK_DATA_CAPACITY 4096
static uint8_t *brainfuck_data_pointer;
static char *brainfuck_instruction_pointer;

void brainfuck_init(char *file_path) {
    brainfuck_data_pointer = malloc(BRAINFUCK_DATA_CAPACITY);
    assert(brainfuck_data_pointer);
    memset(brainfuck_data_pointer, 0, BRAINFUCK_DATA_CAPACITY);
    brainfuck_instruction_pointer = read_file(file_path);
    if(!brainfuck_instruction_pointer) {
        printf("Failed to read file\n");
        exit(1);
    }
}

char brainfuck_next_instruction(void) {
    return *brainfuck_instruction_pointer++;
}

char brainfuck_prev_instruction(void) {
    return *(--brainfuck_instruction_pointer);
}

char brainfuck_input(void) {
    char ret = getchar();
    // skip newline
    getchar();
    return ret;
}

void brainfuck_output(char c) {
    putchar(c);
}

void brainfuck_run() {
    char instruction;
    while((instruction = brainfuck_next_instruction())) {
        switch(instruction) {
            case '>': ++brainfuck_data_pointer; break;
            case '<': --brainfuck_data_pointer; break;
            case '+': ++*brainfuck_data_pointer; break;
            case '-': --*brainfuck_data_pointer; break;
            case '.': brainfuck_output(*brainfuck_data_pointer); break;
            case ',': *brainfuck_data_pointer = brainfuck_input(); break;
            case '[': {
                if(*brainfuck_data_pointer == 0) {
                    int bracket_depth = 0;
                    char c;
                    while((c = brainfuck_next_instruction())) {
                        if(c == ']' && bracket_depth == 0) break;
                        if(c == '[') ++bracket_depth;
                        else if(c == ']') --bracket_depth;
                    }
                }
            } break;
            case ']': {
                if(*brainfuck_data_pointer != 0) {
                    // skip the ] itself
                    brainfuck_prev_instruction();
                    int bracket_depth = 0;
                    char c;
                    while((c = brainfuck_prev_instruction())) {
                        if(c == '[' && bracket_depth == 0) break;
                        if(c == ']') ++bracket_depth;
                        else if(c == '[') --bracket_depth;
                    }
                    // skip the terminating [
                    brainfuck_next_instruction();
                }
            } break;
            default: break;
        }
    }
}

int main(int argc, char **argv) {
    if(argc < 2) {
        printf("Provide file\n");
        return 1;
    }
    char *source_file = argv[1];
    brainfuck_init(source_file);
    brainfuck_run();
}
