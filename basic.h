#pragma once
typedef struct mexec {
    char** commands;
    struct mexec* next;
    char* target;
    int argsNumber;
    int argsCapacity;
} mexec;
