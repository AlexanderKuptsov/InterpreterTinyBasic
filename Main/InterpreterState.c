#include <stdio.h>
#include <stdlib.h>
#include "InterpreterState.h"
#include "Lexemes.h"
#include "TinyVariables.h"


void state_init() {
    main_state.prog_size = PROGRAM_SIZE;
    main_state.current_num_var = 0;
    main_state.tiny_var = (struct variable *) malloc(sizeof(struct variable) * MAX_VARIABLES);

    // Инициализация массива всех меток
    for (int i = 0; i < NUM_LABEL; i++)
        main_state.labels[i].name[0] = '\0';

}

void state_clear(){
     free(main_state.p_buf);
     free(main_state.tiny_var);
     free(tiny_lex.str);
}

void print_error(int e_index) {
    char *e[] = {
            "Syntax error",
            "Unpaired parentheses",
            "Not an expression",
            "Not a variable",
            "Operator required THEN",
            "Attempted to divide by zero",
            "Memory error",
            "Not enough arguments",
            "Syntax error (Expected THEN)",
            "Exceeded amount of labels",
            "GOSUB error",
            "Loading error"
    };

    printf("\nError:\t%s\n", e[e_index]);
    longjmp(main_state.e_buf, 1); // Возврат в точку сохранения
}