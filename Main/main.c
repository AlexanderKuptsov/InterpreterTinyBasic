#include <stdio.h>
#include <stdlib.h>
#include "Analyzer.h"
#include "TinyErrors.h"

#define PROGRAM_SIZE 4096

// Осовные указатели
char *program;  // Указатель программы
char *p_buf;    // Указатель начала буфера программы

// Объявление функции
int load_program(char *, char *);

int main(int argc, char *argInfo[]) {
    char *file_name = argInfo[1]; // Имя файла программы

    if (argc != 2) { // Неправильный ввод
        printf("Try: <executable>.exe <fileName>.txt");
        exit(1);
    }

    // Выделение памяти через malloc
    if ((p_buf = (char *) malloc(PROGRAM_SIZE)) == NULL) {
        print_error(6);
        exit(1);
    }

    // Загрузка программы
    // при ошибке в методе load_program возвращается 0, а значит прекращается работа
    if (!load_program(p_buf, file_name)){
        print_error(11);
        exit(1);
    }

    program = p_buf;
    executing(program);
    return 0;
}

// загрузка программы из файла
int load_program(char *p, char *f_name) {
    FILE *file;
    int i = 0;

    // Читаем файл программы
    if (!(file = fopen(f_name, "r")))
        return 0;

    // Записываем программу в память
    do {
        *p = (char) getc(file); // запись символа из потока filestream
        p++;
        i++;
        if (i == PROGRAM_SIZE)
            p_buf = (char *) realloc(p_buf, (size_t) (i + PROGRAM_SIZE)); // resize memory
    } while (!feof(file));

    if (*(p - 2) == 0x1a) *(p - 2) = '\0'; // файл заканчивается нулевым символом
    else *(p - 1) = '\0'; // Символ конца программы (для завершения)
    fclose(file);

    return 1;
}
