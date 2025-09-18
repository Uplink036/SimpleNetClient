#ifndef MAIN_H
#define MAIN_H

void parseInputArgs(char *argv[], char protocolstring[6], char pathstring[7], char *&destination, char *&destinationPort);
void populateTCPHint(addrinfo *hints);
int handleProtocol(bool foundProtocl, int socketfd, char pathstring[7], char protocolstring[6]);
void getResultResponseBack(int socketfd);
int sendResultToServer(int result, int socketfd);
int getServerTask(int socketfd, char* msg);
bool getServerProtocols(int socketfd, char* expected_protocol);
int calculateServerTask(char* msg);

#endif