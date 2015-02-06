#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdbool.h>
#include <stdlib.h>
#include <strings.h>
#include <pthread.h>
#include <time.h>

#include "player.h"
#include "datatype.h"
#include "question.h"

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

    Player* electedPlayer;	
    Question* currentQuestion;

    int connectedPlayers;
    int maxPlayers;
};

Server* Server_create();
bool Server_run(Server* server, int port);
void Server_waitForClients(Server* server);
void Server_addPlayer(Server* server, int socketID, sockaddr_in* clientInfos);
void Server_electPlayer(Server* server);
void Server_notifyGoodANSW(Server* server, Player* player);

void Server_sendPLID(Server* server, Player* player);  // Player to send id (contained in structure)
void Server_sendPNUM(Server* server, Player* player, bool allowed); // Player allowed to set number of players
void Server_sendELEC(Server* server, Player* player, bool elected);  // Player elec (send also to other)
void Server_sendRESP(Server* server, Player* player, int answerID); // Send resp id to all and winner ID
void Server_sendASKQ(Server* server, Player* player);
void Server_sendASKQtoAll(Server* server); // Send question to all players

void Server_waitForPNUM(Server* server, Player* player); 
void Server_waitForDEFQ(Server* server); // After elec, wait for the question
void Server_waitForANSW(Server* server, Player* player); // After ques, wait for player answer

#endif