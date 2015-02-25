
#include "client.h"

int main(int argc, char **argv) {
  
    // Search server informations
    if (argc != 3) {
       perror("[Main] Usage: client <serveur-ip> <server-port>\n");
       exit(EXIT_FAILURE);
    }
   
    Client* client = Client_create();
    if(client == NULL){
        perror("[Main] Unable to create client");
        exit(EXIT_FAILURE);
    }

    char* serverName = argv[1]; 
    int serverPort = atoi(argv[2]);    
    if(!Client_run(client, serverName, serverPort)){
        perror("[Main] Unable to start connexion");
        exit(EXIT_FAILURE);
    }

    while(true);
}
