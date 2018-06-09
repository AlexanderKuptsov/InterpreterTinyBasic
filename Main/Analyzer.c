#include "Analyzer.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <setjmp.h>
#include <mem.h>
#include "TinyFunctions.h"
#include "Lexemes.h"
#include "TinyErrors.h"


// Переменные
char *program;      // Основной указатель программы
char token[80];     // Строковое представление лексемы
int token_int;      // Внутреннее представление лексемы
int token_type;     // Тип лексемы
jmp_buf e_buf;


struct command {
    char name[7];
    int token_int;
} commands[] = {
        "PRINT", PRINT,
        "INPUT", INPUT,
        "IF", IF,
        "THEN", THEN,
        "GOTO", GOTO,
        "GOSUB", GOSUB,
        "RETURN", RETURN,
        "END", END};

struct variable { // переменная (имя, значение)
    char name[1];
    int value;
} *new_var;


// Объявление функций

int is_delimeter(char c);

int find_command_number(char *t);

void evaluate1(int *result), evaluate2(int *result), evaluate3(int *result), evaluate4(int *result);

void primitive(int *result), arithmetic(char op, int *r, int *s), sign(char op, int *r);


// Начало работы интерпретатора
void executing(char *p) {
    program = p;

    // Инициализация буфера нелокальных переходов
    if (setjmp(e_buf)) exit(1);

    // Ищем метки
    scanLabels(); // для GOTO

    // Главный цикл интерпретатора
    do {
        // получаем токен (лексему)
        token_type = (char) get_token();

        // Проверка на присваивание
        if (token_type == VARIABLE) {
            put_back();
            assignment();
        }

        // Проверка на команду
        if (token_type == COMMAND) {
            switch (token_int) {
                case PRINT:
                    tinyPrint();
                    break;
                case INPUT:
                    tinyInput();
                    break;
                case IF:
                    tinyIf();
                    break;
                case GOTO:
                    tinyGoto();
                    break;
                case GOSUB:
                    tinyGosub();
                    break;
                case RETURN:
                    tinyReturn();
                    break;
                default:
                    break;
            }
        }
    } while (token_int != FINISHED);
}

int get_token() {
    char *temp; //Указатель на лексему

    token_type = 0;
    token_int = 0;
    temp = token;

    //Проверка закончился ли файл интерпретируемой программы
    if (*program == '\0') {
        *token = '\0';
        token_int = FINISHED;
        return (token_type = DELIMITER);
    }

    // Проверка на конец строки программы
    if (*program == '\n') {
        program++; // продвижение по программе
        token_int = EOL;
        *token = '\n';
        temp++;
        *temp = 0;
        return (token_type = DELIMITER);
    }

    // Пропускаем пробелы и табуляцию
    while (isspace(*program))
        program++;

    // Проверка на разделитель
    if (strchr("+-*/%=:,()><", *program)) {
        *temp = *program;
        program++;
        temp++;
        // *temp++ = *program++;
        *temp = 0;
        return (token_type = DELIMITER);
    }

    // Проверяем на строку
    if (*program == '"') {
        program++;
        while (*program != '"' && *program != '\n') *temp++ = *program++;
        if (*program == '\n') print_error(1);
        program++;
        *temp = 0;
        return (token_type = STRING);
    }

    // Проверка на число
    if (isdigit(*program)) {
        while (!is_delimeter(*program)) {
            *temp++ = *program++;
        }
        *temp = 0;
        return (token_type = NUMBER);
    }

    // Проверка на переменную или команду
    if (isalpha(*program)) {
        while (!is_delimeter(*program))
            *temp++ = *program++;
        *temp = 0;

        token_int = find_command_number(token);
        if (!token_int) {
            token_type = VARIABLE;
        } else
            token_type = COMMAND;
        return token_type;
    }
    print_error(0); // nothing
}

int iswhite(char c) {
    if (c == ' ' || c == '\t')
        return 1;
    return 0;
}

int is_delimeter(char c) {
    if (strchr(" !;,+-<>\'/*%=()\"", c) || c == 9 || c == '\r' || c == 0 || c == '\n')
        return 1;
    return 0;
}

// Получение внутреннего представления команды (известной лексемы)
int find_command_number(char *t) {
    // регистр
    for (int i = 0; *commands[i].name; i++) {
        if (!strcmp(commands[i].name, t))
            return commands[i].token_int;
    }
    return 0;  // nothing
}

// Возврат  восстановление лексемы
void put_back() {
    char *t;
    t = token;
    for (; *t; t++) program--;
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
    while ((op = *token) == '+' || op == '-') {
        get_token();
        evaluate2(&store);
        arithmetic(op, result, &store);
    }
}

void evaluate2(int *result) {
    char operation;
    int store;

    evaluate3(result);
    while ((operation = *token) == '*' || operation == '/') {
        get_token();
        evaluate3(&store);
        arithmetic(operation, result, &store);
    }
}

void evaluate3(int *result) {
    char operation;
    operation = 0;

    if ((token_type == DELIMITER) && *token == '+' || *token == '-') {
        operation = *token;
        get_token();
    }

    evaluate4(result);
    if (operation)
        sign(operation, result);
}

void evaluate4(int *result) {
    if ((*token == '(') && (token_type == DELIMITER)) {
        get_token();
        evaluate1(result); // анализируем выражение в скобках
        if (*token != ')')
            print_error(1);
        get_token();
    } else
        primitive(result); // берем значение неделимого эл-а
}

void primitive(int *result) {
    switch (token_type) {
        case VARIABLE:
            *result = findVar(token)->value; // записываем значение переменной
            get_token();
            return;
        case NUMBER:
            *result = atoi(token);           // записываем значение полученного числа
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
            else print_error(6);
            break;
        default:
            break;
    }
}

// получение выражения
void get_exp(int *result) {
    get_token();
    if (!*token) {
        print_error(6);
        return;
    }
    evaluate1(result);
    put_back();
}
