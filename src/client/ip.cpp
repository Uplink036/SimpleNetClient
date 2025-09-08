#include "ip.h"

int split_ip_port_from_string(char* string, char**ip, char** port)
{
    DEBUG_FUNCTION("client::ip::split_ip_port_from_string(%s, %p, %p)\n", string, ip, port)
    char delim = ':';
    int i;
    int inputLen = strlen(string);
    for (i = inputLen; i > 0; i--)
    {
        if (string[i] == delim)
        {
        int destinationLength = i;
        int portLength = inputLen - (i);
        *ip = (char*)malloc(sizeof(destinationLength)*sizeof(**ip));
        strncpy(*ip, string, destinationLength);
        *port = (char*)malloc(sizeof(portLength)*sizeof(**port));
        strncpy(*port, string+i+1, portLength);
        break;
        }
    }
    if (i == 0)
        return -1;
    return 0;
}