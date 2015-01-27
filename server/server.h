#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include "player.h"

#define MAX_PLAYERS 5
#define HOSTNAME_MAX_LENGTH 256
#define QUEUE_MAX_LENGTH 5

typedef struct sockaddr sockaddr;
typedef struct hostent hostent;

typedef struct _Server Server;
struct _Server {
	int socketID;
	Player** players;
    pthread_t* clientsThread;
    int connectedPlayers;
};

Server* Server_create();
bool Server_run(Server* server, int port);
void Server_waitForClients(Server* server);
void Server_addPlayer(Server* server, int socketID, sockaddr_in* clientInfos);

#endif