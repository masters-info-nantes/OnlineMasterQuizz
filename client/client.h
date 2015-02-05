#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdbool.h>
#include <netdb.h>
#include <unistd.h>

#include "datatype.h"
#include "question.h"

#define MAX_BUFFER_SIZE 256

typedef struct sockaddr 	sockaddr;
typedef struct sockaddr_in 	sockaddr_in;
typedef struct hostent 		hostent;

typedef struct _Client Client;
struct _Client {
	int socketID;
	sockaddr_in* socketInfos;
};

Client* Client_create();
bool Client_run(Client* client, char* serverName, int serverPort);

void Client_sendDEFQ(Client* client, Question* question); // After elec, send question if elec = 1
void Client_sendANSW(Client* client, int answer); // After askq, send answer if not elected
void Client_sendPNUM(Client* client, int playersCount);  // If first player, choose how many player in the game

void Client_waitForPLID(Client* client); // Wait player identifier
void Client_waitForPNUM(Client* client);  // Wait to set number of players (if first)
void Client_waitForELEC(Client* client); // After plid, wait for asking question
void Client_waitForASKQ(Client* client); // After elec, wait for question from server if elec = 0
void Client_waitForRESP(Client* client); // After askq or answ, wait answer from server

#endif