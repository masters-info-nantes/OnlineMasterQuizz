
#include "client.h"

int main(int argc, char **argv) {
  
    // Search server informations
    if (argc != 3) {
       perror("[Exit] Usage: client <serveur-ip> <server-port>\n");
       exit(1);
    }
   
    Client* client = Client_create();
    if(client == NULL){
        perror("[Main] Unable to create client");
        exit(1);
    }

    char* serverName = argv[1]; 
    int serverPort = atoi(argv[2]);    
    if(!Client_run(client, serverName, serverPort)){
        perror("[Main] Unable to start connexion");
        exit(1);
    }
    
    Client_waitForId(client);  
}


