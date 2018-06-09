#include "TinyFunctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <mem.h>
#include <ctype.h>
#include "Analyzer.h"
#include "Lexemes.h"
#include "TinyErrors.h"

// Объявление переменных
#define LABEL_LEN   3
#define NUM_LABEL   25
#define SUB_NEST    25

extern char *program;
extern char token[80];
extern int token_int;
extern int token_type;

extern struct variable { // переменная (имя, значение)
    char name[1];
    int value;
} *new_var;

struct label {           // метка (имя, адрес)
    char name[LABEL_LEN];
    char *p;
};
int current_num_var = 0;

struct label labels[NUM_LABEL]; // все метки

char *gStack[SUB_NEST];   // стек подпрограмм GOSUB
int gIndex;               // индекс верхнего эл-а

// поиск переменной
struct variable *findVar(char *n) {
    int i = 1;
    *n = (char) tolower(*n);  // игнорирование регистра
    struct variable *t = new_var;
    while (i <= current_num_var) {
        if (!strcmp(n, t->name)) {
            return t;
        }
        i++;
        t++;
    }
    return NULL;
}

// установка значения
void setVarValue(struct variable *var, int v) {
    var->value = v;
}

// добавление переменной
struct variable *addVar(char *n) {
    struct variable *t = NULL;

    current_num_var++;
    new_var = (struct variable *) realloc(new_var, sizeof(struct variable) * current_num_var);
    t = new_var;

    if (t != NULL) {
        new_var = t;
    }
    int i = 1;
    while (i < current_num_var) { // ищем свободное место
        new_var++;
        i++;
    }
    struct variable *r = new_var;
    *n = (char) tolower(*n);  // игнорирование регистра
    strcpy(new_var->name, n);
    i = 1;
    while (i < current_num_var) { // возвращаемся
        new_var--;
        i++;
    }
    return r;
}


// присваивание значения переменной
void assignment() {
    int value;
    get_token(); // получаем имя переменной

    struct variable *var;
    if ((var = findVar(token)) != NULL) { // такая переменная есть
        get_token(); // пропускаем равно
        get_exp(&value);
        setVarValue(var, value);
    } else { // такой переменной еще нет
        var = addVar(token); // добаляем её
        get_token();         // пропускаем равно
        get_exp(&value);
        setVarValue(var, value);
    }
}

// переход на следующую строку программы
void findEol() {
    while (*program != '\n' && *program != '\0')
        program++;
    if (*program)
        program++;
}

void tinyPrint() {
    int arg;
    char ending = 0;

    do {
        get_token(); // получаем следующий элемент
        if (token_int == EOL || token_int == FINISHED) break;

        if (token_type == STRING) {
            printf(token); // текст
            get_token();
        } else {
            put_back();
            get_exp(&arg);
            get_token();
            printf("%d", arg); // значение выражения
        }
        ending = *token;

        if (*token != ',' && token_int != EOL && token_int != FINISHED)
            print_error(0);
    } while (*token == ',');

    if (token_int == EOL || token_int == FINISHED) {
        if (ending != ',' && ending != ';') printf("\n");
        else print_error(0); // Отсутствует ',' или ';'
    } else print_error(0);
}

void tinyInput() {
    int i;
    struct variable *var;

    get_token();
    if (token_type == STRING) {
        printf(token);
        get_token();
        // если 1-ый аргумент - строка, то проверка запятой
        if (*token != ',') print_error(7);
        get_token();
    } else printf("Write data: "); // 1-ый аргумент по умолчанию
    if ((var = findVar(token)) == NULL)
        var = addVar(token);
    scanf("%d", &i);      // Чтение входных данных
    setVarValue(var, i);  // запись в переменную
}

void tinyIf() {
    int left, right, condition;
    char operation;
    get_exp(&left);               // Получаем левое выражение
    get_token();                  // Получаем оператор
    if (!strchr("=<>", *token)) { // проверка оператора
        print_error(0);           //Недопустимый оператор
        return;
    }
    operation = *token;

    // Определяем результат
    condition = 0;
    switch (operation) {
        case '=':
            get_exp(&right);  // Получаем правое выражение
            if (left == right) condition = 1;
            break;
        case '<':
            get_token();
            if (strchr("=<>", *token)) { // оператор из 2х символов (<=; <>)
                if (*token == '=') {
                    get_exp(&right);  // Получаем правое выражение
                    if (left <= right) condition = 1;
                } else {
                    get_exp(&right);  // Получаем правое выражение
                    if (*token == '>' && left != right) condition = 1;
                }
            } else {
                put_back();
                get_exp(&right);      // Получаем правое выражение
                if (left < right) condition = 1;
            }
            break;
        case '>':
            get_token();
            if (*token == '=') {      // оператор из 2х символов (>=)
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
        if (token_int != THEN) {
            print_error(8);
            return;
        }
    } else {
        // переход к исполняемым командам
        get_token();    
        get_token();
        if (strchr("\n", *token)) {
            do {
                get_token();
            } while (token_int != END);
        } else findEol(); // переход на следующую строку
    }
}

void tinyGoto() {    
    get_token();                 // получаем метку перехода
    program = findLabel(token);  // переход в программе к найденной метки
}

// Инициализация массива всех меток
void labelInit() {
    for (int i = 0; i < NUM_LABEL; i++)
        labels[i].name[0] = '\0';
}

// Поиск всех меток
void scanLabels() {
    int location;
    char *temp;

    labelInit();      // Инициализация массива всех меток
    temp = program;   // сохраняем место начала программы

    get_token();    
    if (token_type == NUMBER) {
        strcpy(labels[0].name, token);
        labels[0].p = program;
    }

    findEol();
    do {
        get_token();
        if (token_type == NUMBER) {
            location = getNextLabel(token);
            if (location == -1 || location == -2) {
                if (location == -1)
                    print_error(9);
                else
                    print_error(0);
            }
            strcpy(labels[location].name, token);
            labels[location].p = program; // текущий указатель программы
        }
        // Если строка не помечена, переход к следующей
        if (token_int != EOL) findEol();
    } while (token_int != FINISHED);
    program = temp; // восстанавливаем начальное значение
}

// Поиск метки
char *findLabel(char *s) {
    for (int i = 0; i < NUM_LABEL; i++)
        if (!strcmp(labels[i].name, s))
            return labels[i].p;
    return '\0';
}

// Поиск последнего индекса массива меток
int getNextLabel(char *s) {
    for (int i = 0; i < NUM_LABEL; i++) {
        if (labels[i].name[0] == 0) return i;
        if (!strcmp(labels[i].name, s)) return -2; // метка уже существует
    }
    return -1; // массив переполнен
}

// GOSUB
void tinyGosub() {
    get_token();
    gPush(program);             // место возвращения (*)
    program = findLabel(token); // переход в найденную метку вызова
}

// Возврат из стека GOSUB
void tinyReturn() {
    program = gPop();           // (*)
}

// Помещает данные в стек GOSUB
void gPush(char *s) {
    gIndex++;
    if (gIndex == SUB_NEST) {
        print_error(10);
        return;
    }
    gStack[gIndex] = s;
}

// Достает данные из стека GOSUB
char *gPop() {
    if (gIndex == 0) {
        print_error(10);
        return '\0';
    }
    return (gStack[gIndex--]);
}
