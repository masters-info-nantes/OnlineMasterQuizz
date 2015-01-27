#include <stdlib.h>
#include <stdio.h>

#include "player.h"
#include <pthread.h>

//#include <linux/types.h>  /* pour les sockets */
//#include <i386/types.h>

#define HOSTNAME_MAX_LENGTH 256
#define QUEUE_MAX_LENGTH 5
#define NUMBER_PLAYER_MAX 5

typedef struct sockaddr sockaddr;
typedef struct hostent hostent;

int createSocket(int port);

int main(int argc, char **argv) {
    
    int socketID = createSocket(5000);
    
    // Listen incoming connexions
    listen(socketID, QUEUE_MAX_LENGTH); 

    // Store clients sockets and threads
    int clientsSocketID[NUMBER_PLAYER_MAX] = { -1 };
    pthread_t clientsThread[NUMBER_PLAYER_MAX];
    int connectedPlayers = 0;

    sockaddr_in* clientInfos = malloc(sizeof(sockaddr_in));
    socklen_t clientInfosSize = sizeof(clientInfos);
  
    // Wait for new incoming connexions
    for(;;) {
    
        // Get client socket
        clientInfosSize = sizeof(clientInfos);
        clientsSocketID[connectedPlayers] = accept(socketID, 
                                                   (sockaddr*)clientInfos,
                                                   &clientInfosSize
        );
        if (clientsSocketID[connectedPlayers] < 0) {
            perror("[Exit] Cannot initiate connexion with a new client");
            exit(1);
        }

        // Create thread dedicated to the new client
        playerinfo* player = malloc(sizeof(playerinfo));
        player->socketID = clientsSocketID[connectedPlayers];
        player->playerID = connectedPlayers; 
        player->networkDetails = clientInfos;

        int threadCreated = pthread_create(&clientsThread[connectedPlayers], 
                                           NULL, clientThread,
                                           (void*)player
        );
        if(threadCreated){
            perror("[Exit] Cannot create thread for new client");
            exit(1);
        }

        connectedPlayers++;
        printf("[Info] New client connected #%d\n", connectedPlayers); 
        printClientInfos(player);  
    } 
}

int createSocket(int port){

    // Get local host name 
    char* serverName[HOSTNAME_MAX_LENGTH + 1] = { 0 };
    gethostname((char*)serverName, HOSTNAME_MAX_LENGTH);       
    
    // Get server informations from name
    hostent* serverInfos = gethostbyname((char*)serverName);
    if (serverInfos == NULL) {
        perror("[Exit] Cannot find server from given hostname");
        exit(1);
    }        

    // Fill socket infos with server infos
    sockaddr_in socketInfos;
    bcopy((char*)serverInfos->h_addr, (char*)&socketInfos.sin_addr, serverInfos->h_length);
    socketInfos.sin_family       = serverInfos->h_addrtype;     /* ou AF_INET */
    socketInfos.sin_addr.s_addr  = INADDR_ANY;           /* ou AF_INET */

    // Socket port configuration and creation
    socketInfos.sin_port = htons(port);
    printf("Listening on: %d\n\n", ntohs(socketInfos.sin_port));

    int socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (socketID < 0) {
        perror("[Exit] Unable to create socket connexion");
        exit(1);
    }

    /* Bind socket identifier with its informations */
    if ((bind(socketID, (sockaddr*)(&socketInfos), sizeof(socketInfos))) < 0) {
        perror("[Exit] Unable to bind connexion address with the socket");
        exit(1);
    }

    return socketID;
}

