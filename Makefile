CFLAGS = -std=c11 -pedantic -Wall -Wextra -c -o
OBJ = Result/main.o Result/InterpreterState.o Result/Lexemes.o Result/Analyzer.o Result/TinyFunctions.o Result/TinyVariables.o
EXE = Result/InterpreterTinyBasic

all: $(EXE)
$(EXE) : $(OBJ)
	gcc -o $(EXE) $(OBJ)

Result/main.o : Main/main.c
	gcc $(CFLAGS) Result/main.o Main/main.c

Result/InterpreterState.o : Main/InterpreterState.c
	gcc $(CFLAGS) Result/InterpreterState.o Main/InterpreterState.c

Result/Lexemes.o : Main/Lexemes.c
	gcc $(CFLAGS) Result/Lexemes.o Main/Lexemes.c

Result/Analyzer.o : Main/Analyzer.c
	gcc $(CFLAGS) Result/Analyzer.o Main/Analyzer.c

Result/TinyFunctions.o : Main/TinyFunctions.c
	gcc $(CFLAGS) Result/TinyFunctions.o Main/TinyFunctions.c

Result/TinyVariables.o : Main/TinyVariables.c
	gcc $(CFLAGS) Result/TinyVariables.o Main/TinyVariables.c
