#include "player.h"

void Player_printClientInfos(Player* player){
    printf("* Remote port: %d\n", player->networkDetails->sin_port);
    printf("* Remote IP: %s\n\n", inet_ntoa(player->networkDetails->sin_addr));    
}

void* Player_clientThread (void* params) {

    void** paramList = (void**) params;
    Player* player = (Player*) paramList[0];
    Server* server = (Server*) paramList[1];

    Server_sendPLID(server, player);

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