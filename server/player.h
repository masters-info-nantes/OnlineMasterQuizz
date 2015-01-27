#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

#define SOCKET_BUFFER_SIZE 256

typedef struct sockaddr_in sockaddr_in;

typedef struct _playerinfo playerinfo;
struct _playerinfo {
    int socketID;
    int playerID;
    sockaddr_in* networkDetails;
};

// Keep void* func(void*) for thread function
void* clientThread(void* playerInfos);
void printClientInfos(playerinfo* player);

#endif