#include "src/client/netparser.h"

void getProtocol(char* input, char* protocol) {
    DEBUG_FUNCTION("client::netparser::getProtocol(%s, %p)\n", input, protocol);
    char *proto_end = strstr(input, "://");
    strncpy(protocol, input, proto_end - input);
    protocol[proto_end - input] = '\0';
    for (int i = 0; i < proto_end - input; i++) {
        protocol[i] = toupper(protocol[i]);
    }
}

void getAPI(char* input, char* api) {
    DEBUG_FUNCTION("client::netparser::getAPI(%s, %p)\n", input, api);
    char *proto_end = strstr(input, "://");
    char *api_start = strstr(proto_end+3, "/");
    api_start += 1;
    int apiLength = strlen(api_start);
    memset(api, 0, apiLength+1);
    strncpy(api, api_start, apiLength);
    DEBUG_FUNCTION("client::netparser::getAPI - %s %s %d\n", api, api_start,
                   apiLength);
    for (int i = 0; i < apiLength; i++) {
        api[i] = toupper(api[i]);
    }
}

void getIPnPORT(char* input, char** ip, char** port) {
    DEBUG_FUNCTION("client::netparser::getIPnPORT(%s, %p, %p)\n", input, ip,
                   port);
    char *proto_end = strstr(input, "://");
    char *ipStart = proto_end+3;
    char *portEnd = strstr(ipStart, "/");
    char *ipNPort = static_cast<char*>(calloc(portEnd-ipStart+1,
                                                   sizeof(char)));
    strncpy(ipNPort, ipStart, portEnd-ipStart);
    DEBUG_FUNCTION("client::netparser::getIPnPORT - ipNPort %s", ipNPort);
    int returnValue = splitIPPortFromString(ipNPort, ip, port);
    free(ipNPort);
    if (returnValue < 0) {
        fflush(stdout);
        printf("Did not find viable IP and PORT, exiting...\n");
        exit(-1);
    }
}
