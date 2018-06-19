#include "TinyVariables.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int current_num_var = 0;

// поиск переменной
struct variable *find_var(char *n) {
    int i = 1;
    *n = (char) tolower(*n);  // игнорирование регистра
    struct variable *t = new_var;
    while (i <= current_num_var) {
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
    struct variable *t = NULL;

    current_num_var++;
    new_var = (struct variable *) realloc(new_var, sizeof(struct variable) * current_num_var);
    t = new_var;

    if (t != NULL) {
        new_var = t;
    }
    int i = 1;
    while (i < current_num_var) { // ищем свободное место
        new_var++;
        i++;
    }
    struct variable *r = new_var;
    *n = (char) tolower(*n);  // игнорирование регистра
    strcpy(new_var->name, n);
    i = 1;
    while (i < current_num_var) { // возвращаемся
        new_var--;
        i++;
    }
    return r;
}