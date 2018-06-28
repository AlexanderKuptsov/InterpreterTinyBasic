#include <stdio.h>
#include "Lexemes.h"
#include "InterpreterState.h"

void initialize_token() {
    tiny_lex.size = INITIAL_TOKEN_SIZE;
    if ((tiny_lex.str = (char *) malloc((size_t) tiny_lex.size * sizeof(char *))) == NULL) {
        print_error(6);
        exit(1);
    }
}

void resize_token() {
    tiny_lex.size += RESIZE_CONST;
    char *new_tok = tiny_lex.str;
    if ((tiny_lex.str = (char *) realloc(tiny_lex.str, (size_t) tiny_lex.size * sizeof(char *))) == NULL) {
        print_error(6);
        exit(1);
    }
    tiny_lex.str = new_tok;
}