#include "TinyFunctions.h"
#include <stdio.h>
#include <string.h>
#include "Analyzer.h"
#include "Lexemes.h"
#include "TinyErrors.h"
#include "TinyVariables.h"
#include "InterpreterState.h"

// Объявление переменных
#define LABEL_LEN   3
#define NUM_LABEL   25
#define SUB_NEST    25

struct label {           // метка (имя, адрес)
    char name[LABEL_LEN];
    char *p;
} labels[NUM_LABEL]; // все метки

char *g_stack[SUB_NEST];   // стек подпрограмм GOSUB
int g_index;               // индекс верхнего эл-а


// присваивание значения переменной
void assignment() {
    int value;
    get_token(); // получаем имя переменной

    struct variable *var;
    if ((var = find_var(tiny_lex.str)) != NULL) { // такая переменная есть
        get_token(); // пропускаем равно
        get_exp(&value);
        set_var_value(var, value);
    } else { // такой переменной еще нет
        var = add_var(tiny_lex.str); // добаляем её
        get_token();         // пропускаем равно
        get_exp(&value);
        set_var_value(var, value);
    }
}

// переход на следующую строку программы
void find_eol() {
    while (*main_state.prog != '\n' && *main_state.prog != '\0')
        main_state.prog++;
    if (main_state.prog)
        main_state.prog++;
}

void tiny_print() {
    int arg;
    char ending = 0;

    do {
        get_token(); // получаем следующий элемент
        if (tiny_lex.index == EOL || tiny_lex.index == FINISHED) break;

        if (tiny_lex.type == STRING) {
            printf(tiny_lex.str); // текст
            get_token();
        } else {
            put_back();
            get_exp(&arg);
            get_token();
            printf("%d", arg); // значение выражения
        }
        ending = *tiny_lex.str;

        if (*tiny_lex.str != ',' && tiny_lex.index != EOL && tiny_lex.index != FINISHED)
            print_error(0);
    } while (*tiny_lex.str == ',');

    if (tiny_lex.index == EOL || tiny_lex.index == FINISHED) {
        if (ending != ',' && ending != ';') printf("\n");
        else print_error(0); // Отсутствует ',' или ';'
    } else print_error(0);
}

void tiny_input() {
    int i;
    struct variable *var;

    get_token();
    if (tiny_lex.type == STRING) {
        printf(tiny_lex.str);
        get_token();
        // если 1-ый аргумент - строка, то проверка запятой
        if (*tiny_lex.str != ',') print_error(7);
        get_token();
    } else printf("Write data: "); // 1-ый аргумент по умолчанию
    if ((var = find_var(tiny_lex.str)) == NULL)
        var = add_var(tiny_lex.str);
    scanf("%d", &i);      // Чтение входных данных
    set_var_value(var, i);  // запись в переменную
}

void tiny_if() {
    int left, right, condition;
    char operation;
    get_exp(&left);               // Получаем левое выражение
    get_token();                  // Получаем оператор
    if (!strchr("=<>", *tiny_lex.str)) { // проверка оператора
        print_error(0);           //Недопустимый оператор
        return;
    }
    operation = *tiny_lex.str;

    // Определяем результат
    condition = 0;
    switch (operation) {
        case '=':
            get_exp(&right);  // Получаем правое выражение
            if (left == right) condition = 1;
            break;
        case '<':
            get_token();
            if (strchr("=<>", *tiny_lex.str)) { // оператор из 2х символов (<=; <>)
                if (*tiny_lex.str == '=') {
                    get_exp(&right);  // Получаем правое выражение
                    if (left <= right) condition = 1;
                } else {
                    get_exp(&right);  // Получаем правое выражение
                    if (*tiny_lex.str == '>' && left != right) condition = 1;
                }
            } else {
                put_back();
                get_exp(&right);      // Получаем правое выражение
                if (left < right) condition = 1;
            }
            break;
        case '>':
            get_token();
            if (*tiny_lex.str == '=') {      // оператор из 2х символов (>=)
                get_exp(&right);      // Получаем правое выражение
                if (left >= right) condition = 1;
            } else {
                put_back();
                get_exp(&right);      // Получаем правое выражение
                if (left > right) condition = 1;
            }
            break;
        default:
            break;
    }
    if (condition) {
        get_token();
        if (tiny_lex.index != THEN) {
            print_error(8);
            return;
        }
    } else {
        // переход к исполняемым командам
        get_token();
        get_token();
        if (strchr("\n", *tiny_lex.str)) {
            do {
                get_token();
            } while (tiny_lex.index != END);
        } else find_eol(); // переход на следующую строку
    }
}

void tiny_goto() {
    get_token();                 // получаем метку перехода
    main_state.prog = find_label(tiny_lex.str);  // переход в программе к найденной метки
}

// Инициализация массива всех меток
void label_init() {
    for (int i = 0; i < NUM_LABEL; i++)
        labels[i].name[0] = '\0';
}

// Поиск всех меток
void scan_labels() {
    int location;
    char *temp;

    label_init();      // Инициализация массива всех меток
    temp = main_state.prog;   // сохраняем место начала программы

    get_token();
    if (tiny_lex.type == NUMBER) {
        strcpy(labels[0].name, tiny_lex.str);
        labels[0].p = main_state.prog;
    }

    find_eol();
    do {
        get_token();
        if (tiny_lex.type == NUMBER) {
            location = get_next_label(tiny_lex.str);
            if (location == -1 || location == -2) {
                if (location == -1)
                    print_error(9);
                else
                    print_error(0);
            }
            strcpy(labels[location].name, tiny_lex.str);
            labels[location].p = main_state.prog; // текущий указатель программы
        }
        // Если строка не помечена, переход к следующей
        if (tiny_lex.index != EOL) find_eol();
    } while (tiny_lex.index != FINISHED);
    main_state.prog = temp; // восстанавливаем начальное значение
}

// Поиск метки
char *find_label(char *s) {
    for (int i = 0; i < NUM_LABEL; i++)
        if (!strcmp(labels[i].name, s))
            return labels[i].p;
    return '\0';
}

// Поиск последнего индекса массива меток
int get_next_label(char *s) {
    for (int i = 0; i < NUM_LABEL; i++) {
        if (labels[i].name[0] == 0) return i;
        if (!strcmp(labels[i].name, s)) return -2; // метка уже существует
    }
    return -1; // массив переполнен
}

// GOSUB
void tiny_gosub() {
    get_token();
    g_push(main_state.prog);             // место возвращения (*)
    main_state.prog = find_label(tiny_lex.str); // переход в найденную метку вызова
}

// Возврат из стека GOSUB
void tiny_return() {
    main_state.prog = g_pop();           // (*)
}

// Помещает данные в стек GOSUB
void g_push(char *s) {
    g_index++;
    if (g_index == SUB_NEST) {
        print_error(10);
        return;
    }
    g_stack[g_index] = s;
}

// Достает данные из стека GOSUB
char *g_pop() {
    if (g_index == 0) {
        print_error(10);
        return '\0';
    }
    return (g_stack[g_index--]);
}
