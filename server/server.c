#include "server.h"

Server* Server_create(){
	Server* server = (Server*) malloc(sizeof(Server));
	
	if(server){
		server->socketID = -1;
		server->connectedPlayers = 0;
        server->maxPlayers = 2;

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
    printf("[Server/Run] Listening on %d\n", ntohs(socketInfos.sin_port));

    server->socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socketID < 0) {
        perror("[Server/Run] Unable to create socket connexion\n");
        return false;
    }

    // Set timeout options
    /*
    struct timeval timeout;      
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    if (setsockopt(server->socketID, SOL_SOCKET, SO_RCVTIMEO, 
            (char *)&timeout, sizeof(timeout)) < 0){
        perror("[Server/Run] Unable set timeout option for client socket\n");
        return false;
    }
    */

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
  
    while(server->connectedPlayers < server->maxPlayers 
            && server->connectedPlayers < 2) {
    
        clientInfosSize = sizeof(clientInfos);
		int socketID = accept(server->socketID, (sockaddr*)clientInfos, &clientInfosSize);

		if (socketID < 0) {
		    perror("[Server/WaitForClients] Cannot initiate connexion with a new client\n");
		    exit(1);
		}

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

    server->players[server->connectedPlayers] = player;
    server->connectedPlayers++;

    printf("\n[Server/WaitForClients] New client connected #%d\n", server->connectedPlayers); 
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
    DataType_plid plid = { player->playerID + 1 }; 

    if(write(player->socketID, &plid, sizeof(plid)) <= 0){
        char message[500];
        sprintf(message, "Cannot send plid to player #%d", player->playerID + 1);
        perror(message);
        exit(0);
    }
     
    printf("> PLID sent to player #%d\n", player->playerID);
}

void Server_sendPNUM(Server* server, Player* player, bool allowed){
    DataType_pnum pnum = { allowed }; 

    if(write(player->socketID, &pnum, sizeof(pnum)) <= 0){
        char message[500];
        sprintf(message, "Cannot send pnum to player #%d", player->playerID + 1);
        perror(message);
        exit(0);
    }
     
    printf("> PNUM %s sent to player #%d\n", (allowed) ? "authorization" : "not authorized", player->playerID);
}

void Server_sendELEC(Server* server, Player* player, bool elected)
{
    ////////////////////////////
    // For mocking reasons!!! //
    ////////////////////////////
    
    DataType_elec elec;
    elec.elected = elected;
    if(write(player->socketID, &elec, sizeof(elec)) <= 0){
        char message[500];
        sprintf(message, "Cannot send elec to player #%d", player->playerID + 1);
        perror(message);
        exit(0);
    }
    printf("player %d elected to select the question!",player->playerID);
    Server_waitForDEFQ(server,player);
}

void Server_sendRESP(Server* server, Player* player, int answerID){

}

void Server_sendASKQtoAll(Server* server, Player* player, Question* question){
    // Exclude elected player
    // Use thread Player_sendASKQ
}

void Server_waitForPNUM(Server* server, Player* player){
    DataType_pnum pnum;

    int readSize = -1;
    while((readSize = read(player->socketID, &pnum, sizeof(pnum))) == 0);

    if(readSize < 0){
        char message[500];
        sprintf(message, "PNUM received is malformed\n");
        perror(message);
        exit(0);
    }

    printf("PNUM set to %d\n", pnum.numberOfPlayers);
    
    ///////////////////////////
    // For mocking reason!!! //
    ///////////////////////////
    
    Server_sendELEC(server,player,1);
}

    ///////////////////////////
    // For mocking reason!!! //
    ///////////////////////////
void Server_waitForDEFQ(Server* server,Player* player){
        DataType_defq defq;
    if(read(player->socketID, &defq, sizeof(defq)) > 0){
        printf("Question recieved!\n");
        printf("%s \n",defq.question);
        printf("%s \n",defq.answer);
    }
}
void Server_waitForASKQ(Server* server,Player* player){

}

void Server_waitForANSW(Server* server, Player* player){

}
