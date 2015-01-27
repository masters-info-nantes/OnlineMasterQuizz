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

void Server_sendPLID(Server* server, Player* player);
void Server_sendELEC(Server* server);
void Server_sendRESP(Server* server, int answerID);
void Server_waitForASKQ(Server* client);
void Server_waitForANSW(Server* client);

#endif