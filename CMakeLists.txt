cmake_minimum_required(VERSION 3.9)
project(InterpreterTinyBasic C)

set(CMAKE_C_STANDARD 11)

add_executable(InterpreterTinyBasic
        Main/main.c
        Main/InterpreterState.h
        Main/Analyzer.c
        Main/Analyzer.h
        Main/TinyFunctions.c
        Main/TinyFunctions.h
        Main/TinyErrors.c
        Main/TinyErrors.h
        Main/TinyVariables.h
        Main/TinyVariables.c
        Main/Lexemes.c Main/Lexemes.h)