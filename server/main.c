
#include "server.h"

int main(int argc, char **argv) {
    
    // Search server informations in args
    if (argc != 2) {
       perror("[Main] Usage: server <port-to-listen>\n");
       exit(EXIT_FAILURE);
    }

    // Create server configuration
    Server* server = Server_create();
    if(server == NULL){
        perror("[Main] Unable to create server");
        exit(EXIT_FAILURE);
    }

    // Run server
    int port = atoi(argv[1]);
    if(!Server_run(server, port)){
        perror("[Main] Unable to start server");
        exit(EXIT_FAILURE);
    }

    // Wait all clients to begin the game
    Server_waitForClients(server);

    // Each turn, elect a player and wait for end game
    while(true){
        Server_waitForEndGame(server);
        Server_electPlayer(server);
    }
}
