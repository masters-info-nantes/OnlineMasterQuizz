
#include "server.h"

int main(int argc, char **argv) {
    
    // Search server informations
    if (argc != 2) {
       perror("[Main] Usage: server <port-to-listen>\n");
       exit(EXIT_FAILURE);
    }

    // Create and run server
    Server* server = Server_create();
    if(server == NULL){
        perror("[Main] Unable to create server");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    if(!Server_run(server, port)){
        perror("[Main] Unable to start server");
        exit(EXIT_FAILURE);
    }

    Server_waitForClients(server);

    while(true){
        Server_waitForGoodAnswers(server);     
    }
}
