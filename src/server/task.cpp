#include <task.h>

bool sendClientTask(int client_fd)
{
  DEBUG_FUNCTION("server::main::sendClientTask(%d)\n", client_fd);
  const char task[] = "add 1 1\n";
  IF_NEGATIVE(send(client_fd, &task, strlen(task), 0))
    return false;
  return true;
}

bool recvClientResponse(int client_fd)
{
  DEBUG_FUNCTION("server::main::recvClientResponse(%d)\n", client_fd);
  char msg[1500];
	static const int max_buffer_size= sizeof(msg)-1;
  int readSize=recv(client_fd,&msg,max_buffer_size,0);
  IF_NEGATIVE(readSize)
    return false;
  IF_NOT_ZERO(strncmp(msg, "2\n", readSize))
    return false;
  return true;
}

bool clientTask(int client_fd)
{
  DEBUG_FUNCTION("server::main::clientTask(%d)\n", client_fd);
  if(sendClientTask(client_fd) EQUALS false)
  {
    close(client_fd);
    return false;
  }
  if(recvClientResponse(client_fd) EQUALS false)
  {
    close(client_fd);
    return false;
  }
  return true;
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