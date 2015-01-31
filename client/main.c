
#include "client.h"

int main(int argc, char **argv) {
  
    // Search server informations
    if (argc != 3) {
       perror("[Main] Usage: client <serveur-ip> <server-port>\n");
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
    
    Client_waitForPLID(client);  

/*
    Client_waitForPNUM(client);
    Client_waitForELEC(client);

    if(elected){
        Client_sendDEFQ(client, question);
    }
    else {
        Client_waitForASKQ(client);
        Client_waitForRESP(client); 

        // Thread terminated if waitForRESP ends
        Client_sendANSW(client, answer);        
    }
*/

}
