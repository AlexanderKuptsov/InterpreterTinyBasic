#include "TinyVariables.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "InterpreterState.h"

// поиск переменной
struct variable *find_var(char *n) {
    int i = 1;
    *n = (char) tolower(*n);  // игнорирование регистра
    struct variable *t = main_state.tiny_var;
    while (i <= main_state.current_num_var) {
        if (!strcmp(n, t->name)) {
            return t;
        }
        i++;
        t++;
    }
    return NULL;
}

// установка значения
void set_var_value(struct variable *var, int v) {
    var->value = v;
}

// добавление переменной
struct variable *add_var(char *n) {
    main_state.current_num_var++;
    struct variable *t = main_state.tiny_var;
    int i = 1;

    while (i < main_state.current_num_var) { // ищем свободное место
        t++;
        i++;
    }
    *n = (char) tolower(*n);  // игнорирование регистра
    strcpy(t->name, n);
    return t;
}