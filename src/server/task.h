#ifndef SRC_SERVER_TASK_H_
#define SRC_SERVER_TASK_H_

#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <memory.h>
#include <cmath>
#include <string>

#include "src/debug.h"
#include "src/lib/calc.h"
#include "src/lib/macros.h"

struct STask {
    op opID;
    int valueOne;
    int valueTwo;
    int result;
};
typedef struct STask STask;

bool sendClientTask(int client_fd, STask* task);
bool recvClientTaskResult(int client_fd, STask* task);
bool clientTask(int client_fd);
STask* getRandomTask();
char* taskToString(STask*);
#endif  // SRC_SERVER_TASK_H_
