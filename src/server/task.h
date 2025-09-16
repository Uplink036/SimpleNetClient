#ifndef TASK_H
#define TASK_H

#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <calc.h>
#include <stdio.h>
#include <memory.h>
#include <cmath>
#include <string> 

#include "debug.h"
#include "macros.h"

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

bool sendClientTask(int client_fd);
bool recvClientResponse(int client_fd);
bool clientTask(int client_fd);
STask* getRandomTask();
char* taskToString(STask*);
#endif