#include "server.h"

Server* Server_create(){
	Server* server = (Server*) malloc(sizeof(Server));
	
	if(server){
		server->socketID = -1;
		server->connectedPlayers = 0;

		server->players = calloc(MAX_PLAYERS, sizeof(Player*));
		if(server->players){
			server->clientsThread = calloc(MAX_PLAYERS, sizeof(pthread_t));

			if(server->clientsThread == NULL){
				free(server->players);
				free(server);
			}
		}
		else {
			free(server);
		}
	}
	
	return server;
}

bool Server_run(Server* server, int port){

    // Get local host name 
    char* serverName[HOSTNAME_MAX_LENGTH + 1] = { 0 };
    gethostname((char*)serverName, HOSTNAME_MAX_LENGTH);       
    
    // Get server informations from name
    hostent* serverInfos = gethostbyname((char*)serverName);
    if (serverInfos == NULL) {
        perror("[Server/Run] Cannot find server from given hostname\n");
        return false;
    }        

    // Fill socket infos with server infos
    sockaddr_in socketInfos;
    bcopy((char*)serverInfos->h_addr, (char*)&socketInfos.sin_addr, serverInfos->h_length);
    socketInfos.sin_family       = serverInfos->h_addrtype;     /* ou AF_INET */
    socketInfos.sin_addr.s_addr  = INADDR_ANY;           /* ou AF_INET */

    // Socket configuration and creation
    socketInfos.sin_port = htons(port);
    printf("[Server/Run] Listening on %d\n\n", ntohs(socketInfos.sin_port));

    server->socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socketID < 0) {
        perror("[Server/Run] Unable to create socket connexion\n");
        return false;
    }

    // Bind socket identifier with its informations
    if ((bind(server->socketID, (sockaddr*)(&socketInfos), sizeof(socketInfos))) < 0) {
        perror("[Server/Run] Unable to bind connexion address with the socket\n");
        return false;
    }

    // Listen incoming connexions
    listen(server->socketID, QUEUE_MAX_LENGTH); 

    return true;
}

void Server_waitForClients(Server* server){
    sockaddr_in* clientInfos = malloc(sizeof(sockaddr_in));
    socklen_t clientInfosSize = sizeof(clientInfos);
  
    // Wait for new incoming connexions: change to LIMIT
    for(;;) {
    
        clientInfosSize = sizeof(clientInfos);
		int socketID = accept(server->socketID, (sockaddr*)clientInfos, &clientInfosSize);

		if (socketID < 0) {
		    perror("[Server/WaitForClients] Cannot initiate connexion with a new client\n");
		    exit(1);
		}

        printf("[Server/WaitForClients] New client connected #%d\n", server->connectedPlayers); 
		Server_addPlayer(server, socketID, clientInfos);
    } 
}

void Server_addPlayer(Server* server, int socketID, sockaddr_in* clientInfos){
    Player* player = malloc(sizeof(Player));
    if(player == NULL){
    	return;
    }

    player->socketID = socketID;
    player->playerID = server->connectedPlayers; 
    player->socketInfos = clientInfos;

    void* threadParams[2] = {player, server};

    // Create thread dedicated to the new client
    int threadCreated = pthread_create(&server->clientsThread[server->connectedPlayers], 
                                       NULL, Player_sendPLID,
                                       (void*)threadParams
    );
    if(threadCreated){
        perror("[Server/AddPlayer] Cannot create thread for new client\n");
        exit(1);
    }

    bool isFirstClient = (server->connectedPlayers == 0) ? true : false;
    Server_sendPNUM(server, player, isFirstClient);

    server->players[server->connectedPlayers] = player;
    server->connectedPlayers++;

    Player_printClientInfos(player);  
}

void Server_electPlayer(Server* server){
/*
    // Use thread Player_sendToElected
    Server_sendELEC(server, player, 1);
    
    for(;;){
        // Use thread Player_sendELEC
        Server_sendELEC(server, player, 0);
    }
*/
}

void Server_notifyGoodANSW(Server* server, Player* player){
    // Stop all threads except caller
    // Use thread Player_sendRESP(params);
}

void Server_sendPLID(Server* server, Player* player){
    DataType_plid plid = { player->playerID + 1 }; // No zero player in display for client
    //DataType_resp plid = { 23, true, "test", 46 };

    if(write(player->socketID, &plid, sizeof(plid)) > 0){

    }
}

void Server_sendPNUM(Server* server, Player* player, bool allowed){

}

void Server_sendELEC(Server* server, Player* player, bool elected){

}

void Server_sendRESP(Server* server, Player* player, int answerID){

}

void Server_sendASKQtoAll(Server* server, Player* player, Question* question){
    // Exclude elected player
    // Use thread Player_sendASKQ
}

void Server_waitForASKQ(Server* server){

}

void Server_waitForANSW(Server* server, Player* player){

}
