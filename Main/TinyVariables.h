#ifndef INTERPRETERTINYBASIC_TINYASSIST_H
#define INTERPRETERTINYBASIC_TINYASSIST_H

struct variable *find_var(char *n);

void set_var_value(struct variable *var, int v);

struct variable *add_var(char *n);

#endif //INTERPRETERTINYBASIC_TINYASSIST_H
