#include "player.h"

Player* Player_create(){
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

void Player_printClientInfos(Player* player){
    printf("* Remote IP: %s\n", inet_ntoa(player->socketInfos->sin_addr));    
    printf("* Remote port: %d\n\n", ntohs(player->socketInfos->sin_port));    
}

void* Player_receive(void* params){
    void** paramList = (void**) params;
    Player* player = (Player*) paramList[0];
    Server* server = (Server*) paramList[1];

    while(Server_receive(server, player));

    player->socketID = -1; // Can't check id thread terminated, so..
    Server_notifyClientDisconnected(server, player);

    free(params);
    return NULL;
}