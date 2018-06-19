#include <stdio.h>
#include <setjmp.h>
#include "TinyErrors.h"
#include "InterpreterState.h"

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

void print_error(int e_index) {
    printf("\nError:\t%s\n", e[e_index]);
    longjmp(main_state.e_buf, 1); // Возврат в точку сохранения
}
