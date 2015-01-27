#include "player.h"

void printClientInfos(playerinfo* player){
    printf("* Remote port: %d\n", player->networkDetails->sin_port);
    printf("* Remote IP: %s\n", inet_ntoa(player->networkDetails->sin_addr));    
}

void* clientThread (void* playerInfos) {

    playerinfo* player = (playerinfo*) playerInfos;
    int clientSocket = player->socketID;

    char buffer[256];
    sprintf(buffer, "%s%d", "PLID", player->playerID); // PLID: player identifier
    write(clientSocket,buffer,strlen(buffer)+1);

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
    return NULL;
}
