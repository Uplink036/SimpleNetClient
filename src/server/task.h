#ifndef TASK_H
#define TASK_H

#include <calc.h>
#include <stdio.h>
#include <memory.h>
#include <cmath>
#include <debug.h>
#include <string> 

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

STask* getRandomTask();
char* taskToString(STask*);
#endif