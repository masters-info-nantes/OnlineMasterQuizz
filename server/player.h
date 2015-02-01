#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

#define SOCKET_BUFFER_SIZE 256

typedef struct sockaddr_in sockaddr_in;

typedef struct _Player Player;
struct _Player {
    int socketID;
    int playerID;
    sockaddr_in* socketInfos;
};

#include "server.h"

// Keep void* func(void*) for thread function
void* Player_sendPLID(void* params);
void* Player_sendToElected(void* params);
void* Player_sendELEC(void* params);
void* Player_sendASKQ(void* params);
void* Player_sendRESP(void* params);

void Player_printClientInfos(Player* player);

#endif