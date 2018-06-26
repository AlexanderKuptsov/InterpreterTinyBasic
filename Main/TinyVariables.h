#ifndef INTERPRETERTINYBASIC_TINYASSIST_H
#define INTERPRETERTINYBASIC_TINYASSIST_H

struct variable { // переменная (имя, значение)
    char name[1];
    int value;
} *new_var;

struct variable *find_var(char *n);

void set_var_value(struct variable *var, int v);

struct variable *add_var(char *n);

#endif //INTERPRETERTINYBASIC_TINYASSIST_H
