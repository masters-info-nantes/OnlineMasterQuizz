#include "player.h"

void Player_printClientInfos(Player* player){
    printf("* Remote port: %d\n", player->networkDetails->sin_port);
    printf("* Remote IP: %s\n\n", inet_ntoa(player->networkDetails->sin_addr));    
}

void* Player_sendPLID (void* params) {

    void** paramList = (void**) params;
    Player* player = (Player*) paramList[0];
    Server* server = (Server*) paramList[1];

    Server_sendPLID(server, player);

    return NULL;
}

void* Player_sendToElected(void* params){
/*
    Server_sendELEC(server, player);
    Server_waitForDEFQ(server);
    Server_sendASKQtoAll(server);
*/
    return NULL;
}

void* Player_sendELEC(void* params){
// Server_sendELEC(server, player);

    return NULL;
}

void* Player_sendASKQ(void* params){
/*
    Server_sendASKQ(server, question);

    while(bad answer && other player don't win){
        Server_waitForANSW(server, player);    

        if(good anwser){
            Server_notifyGoodANSW(server, player);
        }
    }
*/
    return NULL;
}

void* Player_sendRESP(void* params){
//  Server_sendRESP(server, player, anwserID);
    return NULL;
}

/*
    char buffer[256];
    int longueur;
   
    if ((longueur = read(clientSocket, buffer, sizeof(buffer))) <= 0) 
        return NULL;
    
    printf("message lu : %s \n", buffer);
    
    buffer[0] = 'R';
    buffer[1] = 'E';
    buffer[longueur] = '#';
    buffer[longueur+1] ='\0';
    
    printf("message apres traitement : %s \n", buffer);
    
    printf("renvoi du message traite.\n");

    sleep(3);
    
    write(clientSocket,buffer,strlen(buffer)+1);
    
    printf("message envoye. \n");   
    close(clientSocket); 
*/