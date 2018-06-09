#ifndef INTERPRETERTINYBASIC_TINYFUNCTIONS_H
#define INTERPRETERTINYBASIC_TINYFUNCTIONS_H

void assignment();

struct variable *findVar(char *n);

void tinyPrint(), tinyInput(), tinyIf(), tinyGoto(), tinyGosub(), tinyReturn();

void scanLabels();

char *findLabel(char *s);

int getNextLabel(char *s);

void gPush(char *s);

char *gPop();

#endif //INTERPRETERTINYBASIC_TINYFUNCTIONS_H
