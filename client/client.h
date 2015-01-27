#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdbool.h>
#include <netdb.h>
#include <unistd.h>

#include "data.h"

#define SOCKET_BUFFER_SIZE 256

typedef struct sockaddr 	sockaddr;
typedef struct sockaddr_in 	sockaddr_in;
typedef struct hostent 		hostent;

typedef struct _Client Client;
struct _Client {
	int socketID;
};

Client* Client_create();
bool Client_run(Client* client, char* serverName, int serverPort);

void Client_waitForId(Client* client);

#endif