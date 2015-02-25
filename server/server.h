#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdbool.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>

#include "player.h"
#include "datatype.h"
#include "question.h"

#define MAX_PLAYERS 5
#define HOSTNAME_MAX_LENGTH 256
#define QUEUE_MAX_LENGTH 5
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

typedef struct sockaddr sockaddr;
typedef struct hostent hostent;

typedef struct _Server Server;
struct _Server {

	int socketID;
	Player** players;

    Player* electedPlayer;	
    Question* currentQuestion;

    int connectedPlayers;
    int maxPlayers;
    int nbAnswers;
};

Server* Server_create();
bool Server_run(Server* server, int port);
void Server_waitForClients(Server* server);
void Server_addPlayer(Server* server, int socketID, sockaddr_in* clientInfos);
void Server_electPlayer(Server* server);
void Server_notifyANSW(Server* server, Player* player);
void Server_waitForGoodAnswers(Server* server);

void Server_send(Server* server, Player* player, int type, void* data);
void Server_receive(Server* server, Player* player);

void Server_sendPLID(Server* server, Player* player);  // Player to send id (contained in structure)
void Server_sendPNUM(Server* server, Player* player, bool allowed); // Player allowed to set number of players
void Server_sendELEC(Server* server, Player* player, bool elected);  // Player elec (send also to other)
void Server_sendRESP(Server* server, Player* player); // Send resp id to all and winner ID
void Server_sendASKQ(Server* server, Player* player);
void Server_sendASKQtoAll(Server* server); // Send question to all players

void Server_waitForPNUM(Server* server, DataType_pnum pnum); 
void Server_waitForDEFQ(Server* server, DataType_defq defq); // After elec, wait for the question
void Server_waitForANSW(Server* server, Player* player, DataType_answ answ); // After ques, wait for player answer

int Server_levenshteinDistance(char word1[256],char word2[256]);

#endif