#ifndef TASK_H
#define TASK_H

#include <calc.h>
#include <stdio.h>

enum op
{
    ADD,
    DIV,
    MUL, 
    SUB
};

struct STask
{
    op opID;
    int valueOne;
    int valueTwo;
    int result;
};
typedef struct STask STask;

STask getTask();

#endif