#include "ip.h"

bool validPORT(char* port)
{
    DEBUG_FUNCTION("client::ip::validPORT(%s)\n",port);
    int portLength = strlen(port);
    if (!(portLength > 0))
        return false;
    for (int i = 0; i < portLength; i++)
    {
        char c = port[i];
        if (!(c >= '0' && c <='9'))
            return false;
    }
    return true;
}

bool validIP(char* ip)
{
    DEBUG_FUNCTION("client::ip::validIP(%s)\n", ip);
    if (!(strlen(ip) > 0))
        return false;
    return true;
}

int splitIPPortFromString(char* string, char**ip, char** port)
{
    DEBUG_FUNCTION("client::ip::splitIPPortFromString(%s, %p, %p)\n", string, ip, port);
    char delim = ':';
    int i;
    int inputLen = strlen(string);
    for (i = inputLen; i > 0; i--)
    {
        if (string[i] == delim)
        {
        int destinationLength = i;
        int portLength = inputLen - (i);
        *ip = (char*)malloc((destinationLength+2)*sizeof(char));
        memset(*ip, 0, (destinationLength+2)*sizeof(char));
        strncpy(*ip, string, destinationLength);
        *port = (char*)malloc((portLength+2)*sizeof(char));
        memset(*port, 0, (portLength+2)*sizeof(char));
        strncpy(*port, string+i+1, portLength-1);
        break;
        }
    }
    if (i == 0)
        return -1;
    if (!(validPORT(*port) && validIP(*ip)))
        return -2;
    return 0;
}