#include "client.h"

Client* Client_create(){

	Client* client = (Client*)malloc(sizeof(Client));

	if(client){
		client->socketID = -1;
	}

	return client;
}

bool Client_run(Client* client, char* serverName, int serverPort){

    hostent* serverInfos = gethostbyname(serverName);
    if (serverInfos == NULL) {
       perror("[Client/Run] Cannot find server from given hostname\n");
       return false;
    }
    
    // Fill socket infos with server infos
    sockaddr_in socketInfos;
    bcopy((char*)serverInfos->h_addr, (char*)&socketInfos.sin_addr, serverInfos->h_length);
    socketInfos.sin_family = AF_INET;
    socketInfos.sin_port = htons(serverPort);

    // Socket configuration and creation
    printf("[Client/Run] Connect to server %s:%d\n", serverInfos->h_name ,ntohs(socketInfos.sin_port));
        
    client->socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (client->socketID < 0) {
        perror("[Client/Run] Unable to create socket connexion\n");
        return false;
    }
    
    if ((connect(client->socketID, (sockaddr*)(&socketInfos), sizeof(socketInfos))) < 0) {
        perror("[Client/Run] Cannot connect to the server\n");
        return false;
    }
    printf("[Client/Run] Connection sucessfull\n\n");
    
    return true;
}

void Client_waitForId(Client* client){
    char buffer[SOCKET_BUFFER_SIZE];
    DataType queryType = DataType_plid;

    if(read(client->socketID, buffer, queryType.fullSize) > queryType.fullSize){
    	perror("[Client/WaitForId] The maximum number of players on the server has been reached\n");
    }

    Data* response = Data_convertFrom(DataType_plid, buffer);
    if(response == NULL){
        perror("[Client/WaitForId] Server sent something different from player id\n");
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
    