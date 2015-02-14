#include "player.h"

void Player_printClientInfos(Player* player){
    printf("* Remote IP: %s\n", inet_ntoa(player->socketInfos->sin_addr));    
    printf("* Remote port: %d\n\n", ntohs(player->socketInfos->sin_port));    
}

void* Player_receive(void* params){
    void** paramList = (void**) params;
    Player* player = (Player*) paramList[0];
    Server* server = (Server*) paramList[1];

    while(true){
        Server_receive(server, player);
    }

    free(params);
    return NULL;
}