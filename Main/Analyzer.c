#include "Analyzer.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <setjmp.h>
#include <string.h>
#include "InterpreterState.h"
#include "Lexemes.h"
#include "TinyFunctions.h"
#include "TinyVariables.h"

struct command {
    char name[7];
    int token_int;
};

// Объявление функций

int is_delimeter(char c);

int find_command_number(char *t);

void evaluate1(int *result), evaluate2(int *result), evaluate3(int *result), evaluate4(int *result);

void primitive(int *result), arithmetic(char op, int *r, int *s), sign(char op, int *r);


// Начало работы интерпретатора
void executing() {
    printf("\n\r");
    // Инициализация буфера нелокальных переходов
    if (setjmp(main_state.e_buf)) exit(1);
    initialize_token();

    // Ищем метки
    scan_labels(); // для GOTO

    // Главный цикл интерпретатора
    do {
        // получаем токен (лексему)
        tiny_lex.type = (char) get_token();

        // Проверка на присваивание
        if (tiny_lex.type == VARIABLE) {
            put_back();
            assignment();
        }

        // Проверка на команду
        if (tiny_lex.type == COMMAND) {
            switch (tiny_lex.index) {
                case PRINT:
                    tiny_print();
                    break;
                case INPUT:
                    tiny_input();
                    break;
                case IF:
                    tiny_if();
                    break;
                case GOTO:
                    tiny_goto();
                    break;
                case GOSUB:
                    tiny_gosub();
                    break;
                case RETURN:
                    tiny_return();
                    break;
                default:
                    break;
            }
        }
    } while (tiny_lex.index != FINISHED);
}

void count_string_len() {
    int i = 0;
    while (*main_state.prog != '"' && *main_state.prog != '\n') {
        i++;
        if (i >= tiny_lex.size - 1) {
            resize_token();
        }
        main_state.prog++;
    }
    for (int j = 0; j < i; ++j) {
        main_state.prog--;
    }
}

void count_by_delimeter() {
    int i = 0;
    while (!is_delimeter(*main_state.prog)) {
        i++;
        if (i >= tiny_lex.size - 1) {
            resize_token();
        }
        main_state.prog++;
    }
    for (int j = 0; j < i; ++j) {
        main_state.prog--;
    }
}

int get_token() {
    char *temp; //Указатель на лексему
    int i = 0;

    tiny_lex.type = 0;
    tiny_lex.index = 0;
    temp = tiny_lex.str;

    //Проверка закончился ли файл интерпретируемой программы
    if (*main_state.prog == '\0') {
        *tiny_lex.str = '\0';
        tiny_lex.index = FINISHED;
        return (tiny_lex.type = DELIMITER);
    }

    // Проверка на конец строки программы
    if (*main_state.prog == '\n') {
        main_state.prog++; // продвижение по программе
        tiny_lex.index = EOL;
        *tiny_lex.str = '\n';
        temp++;
        *temp = 0;
        return (tiny_lex.type = DELIMITER);
    }

    // Пропускаем пробелы и табуляцию
    while (isspace(*main_state.prog))
        main_state.prog++;

    // Проверка на разделитель
    if (strchr("+-*/%=:,()><", *main_state.prog)) {
        *temp = *main_state.prog;
        main_state.prog++;
        temp++;
        *temp = 0;
        return (tiny_lex.type = DELIMITER);
    }

    // Проверяем на строку
    if (*main_state.prog == '"') {
        main_state.prog++;
        count_string_len(); // преданализ
        temp = tiny_lex.str;
        while (*main_state.prog != '"' && *main_state.prog != '\n') {
            if (i > tiny_lex.size) {
                resize_token();
            }
            *temp++ = *main_state.prog++;
        }
        if (*main_state.prog == '\n') print_error(1);
        main_state.prog++;
        *temp = 0;
        return (tiny_lex.type = STRING);
    }

    // Проверка на число
    if (isdigit(*main_state.prog)) {
        count_by_delimeter();
        temp = tiny_lex.str;
        while (!is_delimeter(*main_state.prog)) {
            *temp++ = *main_state.prog++;
        }
        *temp = 0;
        return (tiny_lex.type = NUMBER);
    }

    // Проверка на переменную или команду
    if (isalpha(*main_state.prog)) {
        count_by_delimeter();
        temp = tiny_lex.str;
        while (!is_delimeter(*main_state.prog)) {
            *temp++ = *main_state.prog++;
        }
        *temp = 0;

        tiny_lex.index = find_command_number(tiny_lex.str);
        if (!tiny_lex.index) {
            tiny_lex.type = VARIABLE;
        } else
            tiny_lex.type = COMMAND;
        return tiny_lex.type;
    }
    print_error(0); // nothing
    return (tiny_lex.index = FINISHED);
}

int is_delimeter(char c) {
    if (strchr(" !;,+-<>\'/*%=()\"", c) || c == 9 || c == '\r' || c == 0 || c == '\n')
        return 1;
    return 0;
}

// Получение внутреннего представления команды (известной лексемы)
int find_command_number(char *t) {
    struct command commands[] = {
            {"PRINT",  PRINT},
            {"INPUT",  INPUT},
            {"IF",     IF},
            {"THEN",   THEN},
            {"GOTO",   GOTO},
            {"GOSUB",  GOSUB},
            {"RETURN", RETURN},
            {"END",    END}};

    for (int i = 0; *commands[i].name; i++) {
        if (!strcmp(commands[i].name, t))
            return commands[i].token_int;
    }
    return 0;  // nothing
}

// Возврат  восстановление лексемы
void put_back() {
    char *t;
    t = tiny_lex.str;
    for (; *t; t++) main_state.prog--;
}

///---Функции рекурсивного синтаксического анализатора---
/* Функции расположены по возрастанию приоритета действий
 * evaluate1: "+" "-"
 * evaluate2: "*" "/"
 * evaluate3: установка знака
 * evaluate4: скобки
 */

void evaluate1(int *result) {
    char op;
    int store;

    evaluate2(result);
    while ((op = *tiny_lex.str) == '+' || op == '-') {
        get_token();
        evaluate2(&store);
        arithmetic(op, result, &store);
    }
}

void evaluate2(int *result) {
    char operation;
    int store;

    evaluate3(result);
    while ((operation = *tiny_lex.str) == '*' || operation == '/') {
        get_token();
        evaluate3(&store);
        arithmetic(operation, result, &store);
    }
}

void evaluate3(int *result) {
    char operation;
    operation = 0;

    if ((tiny_lex.type == DELIMITER && *tiny_lex.str == '+') || (*tiny_lex.str == '-')) {
        operation = *tiny_lex.str;
        get_token();
    }

    evaluate4(result);
    if (operation)
        sign(operation, result);
}

void evaluate4(int *result) {
    if ((*tiny_lex.str == '(') && (tiny_lex.type == DELIMITER)) {
        get_token();
        evaluate1(result); // анализируем выражение в скобках
        if (*tiny_lex.str != ')')
            print_error(1);
        get_token();
    } else
        primitive(result); // берем значение неделимого эл-а
}

void primitive(int *result) {
    switch (tiny_lex.type) {
        case VARIABLE:
            *result = find_var(tiny_lex.str)->value; // записываем значение переменной
            get_token();
            return;
        case NUMBER:
            *result = atoi(tiny_lex.str);           // записываем значение полученного числа
            get_token();
            return;
        default:
            print_error(0);
    }
}

// установка знака
void sign(char op, int *r) {
    if (op == '-') *r = -(*r);
}

// выполнение математических операций
void arithmetic(char op, int *r, int *s) {
    switch (op) {
        case '-':
            *r = *r - *s;
            break;
        case '+':
            *r = *r + *s;
            break;
        case '*':
            *r = *r * *s;
            break;
        case '/':
            if ((*s) != 0)
                *r = (*r) / (*s);
            else print_error(5);
            break;
        default:
            break;
    }
}

// получение выражения
void get_exp(int *result) {
    get_token();
    if (!*tiny_lex.str) {
        print_error(6);
        return;
    }
    evaluate1(result);
    put_back();
}
