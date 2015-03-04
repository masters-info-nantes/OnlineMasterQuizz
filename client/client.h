#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdbool.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

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
	pthread_t* clientThread;
    pthread_t* answerThread;
    pthread_t* questionThread;
    bool       elected;
};

Client* Client_create();
bool Client_run(Client* client, char* serverName, int serverPort);

// Create and run the thread which waits for receiving incoming data
void Client_runReceiveThread(Client* client);

void Client_send(Client* client, int type, void* data);
void Client_receive(Client* client); // Handle incoming requests

// Thread which waits for receiving incoming data
void* Client_threadReceive(void* params);
// Thread which waits for player to send his answer
void* Client_threadAnswer(void* params);
// Thread which waits for player to send his question
void* Client_threadQuestion(void* params);

void Client_sendDEFQ(Client* client, Question* question); // After elec, send question if elec = 1
void Client_sendANSW(Client* client, char answer[256]); // After askq, send answer if not elected
void Client_sendPNUM(Client* client, int playersCount);  // If first player, choose how many player in the game

void Client_waitForPLID(Client* client, DataType_plid plid); // Wait player identifier
void Client_waitForPNUM(Client* client, DataType_pnum pnum);  // Wait to set number of players (if first)
void Client_waitForELEC(Client* client, DataType_elec elec); // After plid, wait for asking question
void Client_waitForASKQ(Client* client, DataType_askq askq); // After elec, wait for question from server if elec = 0
void Client_waitForRESP(Client* client, DataType_resp resp); // After askq or answ, wait answer from server
void Client_waitForENDG(Client* client, DataType_endg endg); 

#endif
