#include <task.h>

int taskResult(STask* task)
{
    DEBUG_FUNCTION("server::task::taskResult(%d, %d, %d)",
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
    DEBUG_FUNCTION("server::task::getRandomTask(%s)", "");
    STask* task = (STask*)malloc(sizeof(*task));
    // memset(task, 0, sizeof(*task));
    task->opID =  op(randomInt() % 4);
    task->valueOne = randomInt();
    task->valueTwo = randomInt();
    task->result = taskResult(task);
    return task;
}