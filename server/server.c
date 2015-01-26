#include <stdlib.h>
#include <stdio.h>

//#include <linux/types.h>  /* pour les sockets */
#include <i386/types.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#define HOSTNAME_MAX_LENGTH 256
#define QUEUE_MAX_LENGTH 5

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

void* clientThread(void* sock_in);
int createSocket(int port);

int main(int argc, char **argv) {
    
    int socketID = createSocket(5000);
    
    // Listen incoming connexions
    listen(socketID, QUEUE_MAX_LENGTH); 

    /* here to avoid loop local variables erasing */
    int nouv_socketID;
    int socketID_thread;
    pthread_t thread_client;

    sockaddr_in clientInfos;
    int clientInfosSize = sizeof(clientInfos);

    /* attente des connexions et traitement des donnees recues */    
    for(;;) {
    
        clientInfosSize = sizeof(clientInfos);
        
        /* clientInfos sera renseigné par accept via les infos du connect */
        nouv_socketID = accept(socketID, (sockaddr*)(&clientInfos),&clientInfosSize);
        if (nouv_socketID < 0) {
            perror("erreur : impossible d'accepter la connexion avec le client.");
            exit(1);
        }
        
        socketID_thread = nouv_socketID;
        if(pthread_create(&thread_client, NULL, clientThread, (void*)(&socketID_thread))){
            perror(">> Erreur lors de la création du thread");
            exit(1);
        }

        /* traitement du message */
        printf("reception d'un message.\n");   
    } 
}

int createSocket(int port){

    // Get local host name 
    char* serverName[HOSTNAME_MAX_LENGTH + 1];
    gethostname(serverName, HOSTNAME_MAX_LENGTH);       
    
    // Get server informations from name
    hostent* serverInfos = gethostbyname(serverName);
    if (serverInfos == NULL) {
        perror("erreur : impossible de trouver le serveur a partir de son nom.");
        exit(1);
    }        

    // Fill socket infos with server infos
    sockaddr_in socketInfos;
    bcopy((char*)serverInfos->h_addr, (char*)&socketInfos.sin_addr, serverInfos->h_length);
    socketInfos.sin_family       = serverInfos->h_addrtype;     /* ou AF_INET */
    socketInfos.sin_addr.s_addr  = INADDR_ANY;           /* ou AF_INET */

    // Socket port configuration and creation
    socketInfos.sin_port = htons(port);
    printf("numero de port pour la connexion au serveur : %d \n", ntohs(socketInfos.sin_port));

    int socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (socketID < 0) {
        perror("erreur : impossible de creer la socket de connexion avec le client.");
        exit(1);
    }

    /* Bind socket identifier with its informations */
    if ((bind(socketID, (sockaddr*)(&socketInfos), sizeof(socketInfos))) < 0) {
        perror("erreur : impossible de lier la socket a l'adresse de connexion.");
        exit(1);
    }

    return socketID;
}

void* clientThread (void* sock_in) {

    printf("Passage dans le thread\n");
    int* tmp = (int*)sock_in;
    int sock = *tmp;

    char buffer[256];
    int longueur;
   
    if ((longueur = read(sock, buffer, sizeof(buffer))) <= 0) 
        return NULL;
    
    printf("message lu : %s \n", buffer);
    
    buffer[0] = 'R';
    buffer[1] = 'E';
    buffer[longueur] = '#';
    buffer[longueur+1] ='\0';
    
    printf("message apres traitement : %s \n", buffer);
    
    printf("renvoi du message traite.\n");

    /* mise en attente du prgramme pour simuler un delai de transmission */
    sleep(3);
    
    write(sock,buffer,strlen(buffer)+1);
    
    printf("message envoye. \n");   
    close(sock); 
}
