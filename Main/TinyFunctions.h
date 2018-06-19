#ifndef INTERPRETERTINYBASIC_TINYFUNCTIONS_H
#define INTERPRETERTINYBASIC_TINYFUNCTIONS_H

void assignment();

void tiny_print(), tiny_input(), tiny_if(), tiny_goto(), tiny_gosub(), tiny_return();

void scan_labels();

char *find_label(char *s);

int get_next_label(char *s);

void g_push(char *s);

char *g_pop();

#endif //INTERPRETERTINYBASIC_TINYFUNCTIONS_H
