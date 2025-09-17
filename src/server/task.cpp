#include <task.h>

bool sendClientTask(int client_fd, STask* task)
{
    DEBUG_FUNCTION("server::task::sendClientTask(%d)\n", client_fd);
    const char* taskString = taskToString(task);
    const int stringLength = strlen(taskString);
    const int noBytesSent = send(client_fd, taskString, stringLength, 0);
    IF_NEGATIVE(noBytesSent)
      return false;
    return true;
}

bool sendClientSuccess(int client_fd)
{
  DEBUG_FUNCTION("server::task::sendClientSuccess(%d)\n", client_fd);
  const char successMessage[] = "OK\n";
  const int stringLength = strlen(successMessage);
  const int noBytesSent = send(client_fd, successMessage, stringLength, 0);
  IF_NEGATIVE(noBytesSent)
    return false;
  return true;
}

bool sendClientFail(int client_fd)
{
  DEBUG_FUNCTION("server::task::sendClientFail(%d)\n", client_fd);
  const char failMessage[] = "ERROR\n";
  const int stringLength = strlen(failMessage);
  const int noBytesSent = send(client_fd, failMessage, stringLength, 0);
  IF_NEGATIVE(noBytesSent)
    return false;
  return true;
}

bool recvClientTaskResult(int client_fd, STask* task)
{
    DEBUG_FUNCTION("server::task::recvClientTaskResult(%d)\n", client_fd);
    char msg[1500];
    static const int max_buffer_size= sizeof(msg)-1;
    int readSize=recv(client_fd,&msg,max_buffer_size,0);
    IF_NEGATIVE(readSize)
      return false;
    char* endPointer = msg+readSize;
    int result = (int)strtol(msg, &endPointer, 10);
    DEBUG_FUNCTION("server::task::recvClientTaskResult - Result %d Task %d Truth %d\n", result, task->result, (result==task->result));
    IF_ZERO((result == task->result))
      sendClientFail(client_fd);
    else
      sendClientSuccess(client_fd);
    return true;
}

bool clientTask(int client_fd)
{
    DEBUG_FUNCTION("server::task::clientTask(%d)\n", client_fd);
    STask* task = getRandomTask(); 
    bool success = false;
    if(sendClientTask(client_fd, task) EQUALS true)
    {
      if(recvClientTaskResult(client_fd, task) EQUALS true)
      {
        success = true;
      }
    }
    close(client_fd);
    free(task);
    return success;
}

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