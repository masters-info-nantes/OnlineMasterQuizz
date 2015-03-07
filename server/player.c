#include "player.h"

// Allocate memory for player struct
Player* Player_create(){

    // Memory allocation
    Player* player = (Player*) malloc(sizeof(Player));
    
    if(player){
        player->socketID = -1;
        player->socketID = -1;
        player->score = 0;
        player->socketInfos = (sockaddr_in*) malloc(sizeof(sockaddr_in));

        if(player->socketInfos){
            player->waitingThread = (pthread_t*) malloc(sizeof(pthread_t));

            if(player->waitingThread == NULL){
                free(player->socketInfos);
                free(player);
            }
        }
        else {
            free(player);
        }
    }

    return player;
}

// Print player network infos
void Player_printClientInfos(Player* player){
    printf("* Remote IP: %s\n", inet_ntoa(player->socketInfos->sin_addr));    
    printf("* Remote port: %d\n\n", ntohs(player->socketInfos->sin_port));    
}

// Thread which waits for client incoming data
void* Player_receive(void* params){

    // Get server and player from params
    void** paramList = (void**) params;
    Player* player = (Player*) paramList[0];
    Server* server = (Server*) paramList[1];

    // Infinite waiting until the client disconnexion
    while(Server_receive(server, player));

    // Notify server the client is disconnected
    player->socketID = -1; // Can't check id thread terminated, so..
    Server_notifyClientDisconnected(server, player);

    free(params);
    return NULL;
}