#pragma once
typedef struct mexec {
    char** commands;
    struct mexec* next;
    char* target;
    int argsNumber;
    int argsCapacity;
} mexec;


typedef struct groupexec {
    char** targets;
    char* name;
    struct groupexec* next;
    int numbers;
    int capacity;
} groupexec;