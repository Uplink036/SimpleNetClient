#include <task.h>

int taskResult(STask* task)
{
    DEBUG_FUNCTION("server::task::taskResult(%d, %d, %d)\n",
                    task->opID, task->valueOne, task->valueTwo);
    int opResult;
    double temp;
    switch (task->opID)
    {
        case op::ADD:
            opResult = task->valueOne + task->valueTwo;
            break;
        case op::SUB:
            opResult = task->valueOne - task->valueTwo;
            break;
        case op::MUL:
            opResult = task->valueOne * task->valueTwo;
            break;
        case op::DIV:
            temp = task->valueOne / task->valueTwo;
            opResult = round(temp);
            break;
        default:
            perror("Found an unidentified op");
            return -1;
            break;
    }
    return opResult;
}

STask* getRandomTask()
{
    DEBUG_FUNCTION("server::task::getRandomTask(%s)\n", "");
    STask* task = (STask*)malloc(sizeof(*task));
    memset(task, 0, sizeof(*task));
    task->opID =  op(randomInt() % 4);
    task->valueOne = randomInt();
    task->valueTwo = randomInt();
    task->result = taskResult(task);
    return task;
}

char* taskToString(STask* task)
{
    DEBUG_FUNCTION("server::task::taskToString(%d, %d, %d)\n",
                    task->opID, task->valueOne, task->valueTwo);
    const size_t stringSize = 31+1;
    char* taskString = (char*)calloc(stringSize, sizeof(char));
    snprintf(taskString, stringSize, "%s %d %d\n",
            arith[task->opID],
            task->valueOne,
            task->valueTwo);
    return taskString;
}