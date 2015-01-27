#include <stdlib.h>
#include <stdio.h>

//#include <linux/types.h>
//#include <i386/types.h>

//#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_BUFFER_SIZE 256
#define QUERY_PLID_LENGTH 5

typedef struct sockaddr 	sockaddr;
typedef struct sockaddr_in 	sockaddr_in;
typedef struct hostent 		hostent;

typedef struct _data data;
struct _data {
    char* type;
    char* content;
};

int createSocket(char* serverName, int port);
data* formatData(char* dataToFormat);

int main(int argc, char **argv) {
  
    // Search server informations
    if (argc != 3) {
       perror("[Exit] Usage: client <serveur-ip> <server-port>\n");
       exit(1);
    }
   
    char* serverName = argv[1]; 
    int serverPort = atoi(argv[2]);    
    int socketID = createSocket(serverName, serverPort);

    // Waiting for player id
    char buffer[SOCKET_BUFFER_SIZE];
    int responseLength = -1;

    if((responseLength = read(socketID, buffer, QUERY_PLID_LENGTH)) > 0) {
        data* response = formatData(buffer);
        if(strcmp(response->type, "PLID") != 0){
            perror("[Exit] The maximum number of players on the server has been reached\n");
            exit(1);
        }
        printf("You are player #%d\n", atoi(response->content));  
    }
    
 /*  
    printf("envoi d'un message au serveur. \n");
      
    if ((write(socket_descriptor, mesg, strlen(mesg))) < 0) {
	perror("erreur : impossible d'ecrire le message destine au serveur.");
	exit(1);
    }
    

    sleep(3);
     
    printf("message envoye au serveur. \n");
                

    while((longueur = read(socket_descriptor, buffer, sizeof(buffer))) > 0) {
	printf("reponse du serveur : \n");
	write(1,buffer,longueur);
    }
    
    printf("\nfin de la reception.\n");
    
    close(socket_descriptor);
    
    printf("connexion avec le serveur fermee, fin du programme.\n");
    
    exit(0);
 */   
}

int createSocket(char* serverName, int port){
 
    hostent* serverInfos = gethostbyname(serverName);
    if (serverInfos == NULL) {
       perror("[Exit] Cannot find server from given hostname\n");
       exit(1);
    }
    
    // Fill socket infos with server infos
    sockaddr_in socketInfos;
    bcopy((char*)serverInfos->h_addr, (char*)&socketInfos.sin_addr, serverInfos->h_length);
    socketInfos.sin_family = AF_INET;
    socketInfos.sin_port = htons(port);

    // Socket configuration and creation
    printf("Connect to server %s:%d\n", serverInfos->h_name ,ntohs(socketInfos.sin_port));
        
    int socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (socketID < 0) {
        perror("[Exit] Unable to create socket connexion\n");
        exit(1);
    }
    
    if ((connect(socketID, (sockaddr*)(&socketInfos), sizeof(socketInfos))) < 0) {
        perror("[Exit] Cannot connect to the server\n");
        exit(1);
    }
    printf("Connection sucessfull\n\n");
    
    return socketID;
}

data* formatData(char* dataToFormat){
    data* formatedData = malloc(sizeof(data));
    formatedData->type = calloc(4 + 1, sizeof(char));
    formatedData->content = calloc(SOCKET_BUFFER_SIZE + 1, sizeof(char));

    memcpy(formatedData->type, dataToFormat, 4);
    memcpy(formatedData->content, dataToFormat + 4, 1);

    return formatedData;
}
