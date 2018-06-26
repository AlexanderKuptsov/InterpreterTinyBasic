#include <stdio.h>
#include "Lexemes.h"
#include "TinyErrors.h"

void initialize_token() {
    tiny_lex.size = INITIAL_TOKEN_SIZE;
    if ((tiny_lex.str = (char *) malloc(tiny_lex.size * sizeof(char *))) == NULL) {
        print_error(6);
        exit(1);
    }
}

void resize_token() {
    tiny_lex.size += RESIZE_CONST;
    char *new_tok = (char *) realloc(tiny_lex.str, tiny_lex.size * sizeof(char *));
    if (new_tok == NULL) {
        print_error(6);
        exit(1);
    } else {
        tiny_lex.str = new_tok;
    }
}

void clear_token() {
    tiny_lex.size = INITIAL_TOKEN_SIZE;
    char *new_tok = (char *) realloc(tiny_lex.str,tiny_lex.size * sizeof(char *));
    if (new_tok == NULL) {
        print_error(6);
        exit(1);
    } else {
        tiny_lex.str = new_tok;
    }
}