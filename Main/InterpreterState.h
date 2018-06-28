#ifndef INTERPRETERTINYBASIC_INTERPRETERSTATE_H
#define INTERPRETERTINYBASIC_INTERPRETERSTATE_H

#include <setjmp.h>

#define PROGRAM_SIZE 4096
#define MAX_VARIABLES 26
#define LABEL_LEN   3
#define NUM_LABEL   25
#define SUB_NEST    25

struct label {           // метка (имя, адрес)
    char name[LABEL_LEN];
    char *p;
};

struct gosub {
    char *g_stack[SUB_NEST];   // стек подпрограмм GOSUB
    int g_index;               // индекс верхнего эл-а
};

struct variable { // переменная (имя, значение)
    char name[1];
    int value;
};

struct state {
    char *prog;      // Основной указатель программы
    char *p_buf;     // Указатель начала буфера программы
    jmp_buf e_buf;
    size_t prog_size;
    int current_num_var;
    struct label labels[NUM_LABEL];
    struct gosub g_info;
    struct variable *tiny_var;
} main_state;

void state_init();

void state_clear();

void print_error(int);

#endif //INTERPRETERTINYBASIC_INTERPRETERSTATE_H
