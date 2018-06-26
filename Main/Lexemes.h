#ifndef INTERPRETERTINYBASIC_LEXEMES_H
#define INTERPRETERTINYBASIC_LEXEMES_H

// Основные типы лексем
#include <stdlib.h>

#define DELIMITER  1
#define VARIABLE   2
#define NUMBER     3
#define COMMAND    4
#define STRING     5
//
// Внутренние представления команд
#define PRINT      10
#define INPUT      11
#define IF         12
#define THEN       13
#define GOTO       14
#define GOSUB      15
#define RETURN     16
#define EOL        17
#define END        18
#define FINISHED   19

#define INITIAL_TOKEN_SIZE 5;
#define RESIZE_CONST 10;

struct token {
    char *str;   // Строковое представление лексемы
    int index;      // Внутреннее представление лексемы
    int type;       // Тип лексемы
    int size;
} tiny_lex;

void initialize_token(), resize_token(), clear_token();

#endif //INTERPRETERTINYBASIC_LEXEMES_H
