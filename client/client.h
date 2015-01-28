#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdbool.h>
#include <netdb.h>
#include <unistd.h>

#include "data.h"
#include "question.h"

typedef struct sockaddr 	sockaddr;
typedef struct sockaddr_in 	sockaddr_in;
typedef struct hostent 		hostent;

typedef struct _Client Client;
struct _Client {
	int socketID;
};

Client* Client_create();
bool Client_run(Client* client, char* serverName, int serverPort);

void Client_sendDEFQ(Client* client, Question* question); // After elec, send question if elec = 1
void Client_sendANSW(Client* client, int answer); // After askq, send answer if not elected

void Client_waitForPLID(Client* client); // Wait player identifier
void Client_waitForELEC(Client* client); // After plid, wait for asking question
void Client_waitForASKQ(Client* client); // After elec, wait for question from server if elec = 0
void Client_waitForRESP(Client* client); // After askq or answ, wait answer from server

#endif