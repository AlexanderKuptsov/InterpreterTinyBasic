#ifndef INTERPRETERTINYBASIC_INTERPRETERSTATE_H
#define INTERPRETERTINYBASIC_INTERPRETERSTATE_H

#include <setjmp.h>

#define PROGRAM_SIZE 4096

struct state {
    char *prog;      // Основной указатель программы
    jmp_buf e_buf;
    char *p_buf;     // Указатель начала буфера программы
    size_t prog_size;
} main_state;

#endif //INTERPRETERTINYBASIC_INTERPRETERSTATE_H
