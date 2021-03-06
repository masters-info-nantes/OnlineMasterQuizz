#include "server.h"

// Allocate memory for server structure
Server* Server_create(){

    // Allocate memory
	Server* server = (Server*) malloc(sizeof(Server));
	
	if(server){
        Server_reset(server);
		server->socketID = -1;
		server->connectedPlayers = 0;
        server->playersForTurn = 0;
        server->maxPlayers = 1;

		server->players = calloc(MAX_PLAYERS, sizeof(Player*));
		if(server->players == NULL){			
			free(server);
		}
	}

    // Initialize random value
    srand(time(NULL));
	
	return server;
}

// Reset server values for next turn
void Server_reset(Server* server){
    server->electedPlayer = NULL;
    server->nbAnswers = 0;  
}

// Initialize server configuration
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
    bcopy((char*)serverInfos->h_addr_list[0], (char*)&socketInfos.sin_addr, serverInfos->h_length);
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

    // Bind socket identifier with its informations
    if ((bind(server->socketID, (sockaddr*)(&socketInfos), sizeof(socketInfos))) < 0) {
        perror("[Server/Run] Unable to bind connexion address with the socket\n");
        return false;
    }

    // Listen incoming connexions
    listen(server->socketID, QUEUE_MAX_LENGTH); 

    return true;
}

// Wait all clients before begining a new game
void Server_waitForClients(Server* server){
    
    sockaddr_in* clientInfos = malloc(sizeof(sockaddr_in));
    socklen_t clientInfosSize = sizeof(clientInfos);

    // While players are missing, wait new ones
    while(server->connectedPlayers < server->maxPlayers) {
    
        clientInfosSize = sizeof(clientInfos);
		int socketID = accept(server->socketID, (sockaddr*)clientInfos, &clientInfosSize);

		if (socketID < 0) {
		    perror("[Server/WaitForClients] Cannot initiate connexion with a new client\n");
		    exit(1);
		}

        // Add new player on server
		Server_addPlayer(server, socketID, clientInfos);

        // Wait for the first set number of players
        if(server->connectedPlayers == 1){
            while(server->maxPlayers == 1);
        }
    } 
    
    printf("> No places left for this turn !\n\n");
    Server_electPlayer(server);
}

// Add a new player to the server
void Server_addPlayer(Server* server, int socketID, sockaddr_in* clientInfos){

    // Create player struct
    Player* player = Player_create();
    if(player == NULL){
    	return;
    }

    player->socketID = socketID;
    player->playerID = server->connectedPlayers; 
    player->socketInfos = clientInfos;


    // Create thread to listen new client
    void** threadParams = (void**) calloc(2, sizeof(void*));
    threadParams[0] = player;
    threadParams[1] = server;

    int threadCreated = pthread_create(player->waitingThread, 
                                       NULL, Player_receive,
                                       (void*)threadParams
    );

    if(threadCreated){
        perror("[Server/AddPlayer] Cannot create thread for new client\n");
        exit(EXIT_FAILURE);
    }

    printf("\n[Server/WaitForClients] New client connected #%d\n", server->connectedPlayers); 
    Player_printClientInfos(player);  

    // Update server infos
    server->players[server->connectedPlayers] = player;
    server->connectedPlayers++;
    server->playersForTurn++;

    // Send player id and ask for to set number of players (if first)
    Server_sendPLID(server, player);

    bool isFirstClient = (player->playerID == 0) ? true : false;
    Server_sendPNUM(server, player, isFirstClient);
}

// Elect a player to ask the question
void Server_electPlayer(Server* server){
    int electedID = rand() % (server->connectedPlayers - 1);

    // Send to all player if they has been elected or not
    for(int i = 0; i < server->connectedPlayers; i++){
        Player* currentPlayer = server->players[i];

        bool elected = false;
        if(i == electedID){
            server->electedPlayer = server->players[electedID];
            elected = true;
        }

        // printf(">> Player #%d has%s been elected to ask the question\n", currentPlayer->playerID + 1, (i == electedID) ? "" : " not");
        Server_sendELEC(server, currentPlayer, elected);
    }
}

// Send turn result to all players
void Server_notifyANSW(Server* server, Player* player){
    if(player != NULL){
        printf("> Player #%d has the good answer\n", player->playerID + 1);        
    }
    else {
        printf("> Nobody found the good answer\n");
    }

    // Send to all players
    for(int i = 0; i < server->playersForTurn; i++){
        Player* currentPlayer = server->players[i];

        if(currentPlayer->socketID == -1){
            continue; // disconnected client
        }

        // Update winner score
        if(player != NULL 
            && currentPlayer->playerID == player->playerID)
        {
            player->score++;
        }
        Server_sendRESP(server, currentPlayer);
    }

    Server_reset(server);
}

// Send a trame to a player
void Server_send(Server* server, Player* player, int type, void* data){

    // If the player is disconnected, don't send
    if(player->socketID == -1){
        return;
    }

    // First trame: notify data type
    DataType typeNotif = { type };
    if(write(player->socketID, &typeNotif, sizeof(typeNotif)) <= 0){
        char message[500];
        sprintf(message, "[Server/send] Cannot send type TYPE to #%d", player->playerID + 1);
        perror(message);
        exit(EXIT_FAILURE);
    }

    // Second trame: data
    switch(typeNotif.type){
        case DATATYPE_PLID: {
            DataType_plid plid = *((DataType_plid*)data);

            if(write(player->socketID, &plid, sizeof(plid)) <= 0){
                char message[500];
                sprintf(message, "[Server/send] Cannot send PLID to #%d", player->playerID + 1);
                perror(message);
                exit(EXIT_FAILURE);
            }
        }
        break;

        case DATATYPE_PNUM: {
            DataType_pnum pnum = *((DataType_pnum*)data);

            if(write(player->socketID, &pnum, sizeof(pnum)) <= 0){
                char message[500];
                sprintf(message, "[Server/send] Cannot send PNUM to #%d", player->playerID + 1);
                perror(message);
                exit(EXIT_FAILURE);
            }
        }
        break; 

        case DATATYPE_ELEC: {
            DataType_elec elec = *((DataType_elec*)data);

            if(write(player->socketID, &elec, sizeof(elec)) <= 0){
                char message[500];
                sprintf(message, "[Server/send] Cannot send ELEC to #%d", player->playerID + 1);
                perror(message);
                exit(EXIT_FAILURE);
            }
        }
        break;

        case DATATYPE_ASKQ: {
            DataType_askq askq = *((DataType_askq*)data);

            if(write(player->socketID, &askq, sizeof(askq)) <= 0){
                char message[500];
                sprintf(message, "[Server/send] Cannot send ASKQ to #%d", player->playerID + 1);
                perror(message);
                exit(EXIT_FAILURE);
            }
        }
        break;

        case DATATYPE_RESP: {
            DataType_resp resp = *((DataType_resp*)data);

            if(write(player->socketID, &resp, sizeof(resp)) <= 0){
                char message[500];
                sprintf(message, "[Server/send] Cannot send RESP to #%d", player->playerID + 1);
                perror(message);
                exit(EXIT_FAILURE);
            }
        }
        break;    

        case DATATYPE_ENDG: {
            DataType_endg endg = *((DataType_endg*)data);

            if(write(player->socketID, &endg, sizeof(endg)) <= 0){
                char message[500];
                sprintf(message, "[Server/send] Cannot send ENDG to #%d", player->playerID + 1);
                perror(message);
                exit(EXIT_FAILURE);
            }
        }
        break; 
    }  
}

// Receive data from a player
bool Server_receive(Server* server, Player* player){
    DataType typeNotif;

    // Read next trame type
    int clientStatus = read(player->socketID, &typeNotif, sizeof(typeNotif));

    // Wait for data trame
    if(clientStatus > 0){
        switch(typeNotif.type){

            case DATATYPE_PNUM: {
                DataType_pnum pnum;
                clientStatus = read(player->socketID, &pnum, sizeof(pnum));

                if(clientStatus > 0){
                    Server_waitForPNUM(server, pnum);
                }
            }
            break;

            case DATATYPE_DEFQ: {
                DataType_defq defq;
                clientStatus = read(player->socketID, &defq, sizeof(defq));

                if(clientStatus > 0){
                    Server_waitForDEFQ(server, defq);                   
                }
            }
            break;

            case DATATYPE_ANSW: {
                DataType_answ answ;
                clientStatus = read(player->socketID, &answ, sizeof(answ));

                if(clientStatus > 0){
                    Server_waitForANSW(server, player, answ);
                }
            }
            break;                                                
        }        
    } 

    return clientStatus > 0;
}

// Blocks until the turn is not ended
void Server_waitForEndGame(Server* server){
    while(server->electedPlayer != NULL);
}

// Called when a client has been disconnected
void Server_notifyClientDisconnected(Server* server, Player* player){
    printf("> Player #%d leave the game\n", player->playerID + 1);
    server->connectedPlayers--;

    // If the game can't go through, notify players for and game
    if(server->connectedPlayers < 2 && server->electedPlayer->socketID != -1){
        for(int i = 0; i < server->playersForTurn; i++){
            Player* currentPlayer = server->players[i];
            Server_sendENDG(server, currentPlayer);
        }
    }
}

/***************************  Function for each request type ****************************/
// Send player identifier
void Server_sendPLID(Server* server, Player* player){
    DataType_plid plid = { DATATYPE_PLID, player->playerID + 1 }; 
    Server_send(server, player, DATATYPE_PLID, &plid);     
    printf("> PLID sent to player #%d\n", player->playerID + 1);
}

// Ask a player to set number of players
void Server_sendPNUM(Server* server, Player* player, bool allowed){
    DataType_pnum pnum = { DATATYPE_PNUM, allowed};
    Server_send(server, player, DATATYPE_PNUM, &pnum);     
    printf("> PNUM %s sent to player #%d\n", (pnum.numberOfPlayers == 1) ? "authorization" : "not authorized", player->playerID + 1);
}

// Notify a player if he has been elected or not
void Server_sendELEC(Server* server, Player* player, bool elected){
    DataType_elec elec = { DATATYPE_ELEC, elected };
    Server_send(server, player, DATATYPE_ELEC, &elec);
    printf("> Player #%d has%s been elected to ask the question\n", player->playerID + 1, (elected) ? "" : " not");
}

// Send response to the question (and score) to a player
void Server_sendRESP(Server* server, Player* player){
    DataType_resp resp;
    resp.type = DATATYPE_RESP;
    resp.score = player->score;    
    strcpy(resp.answer, server->currentQuestion->goodAnswer);

    Server_send(server, player, DATATYPE_RESP, &resp);
    printf("> Send response to player #%d (has %d score)\n", player->playerID + 1, player->score);
}

// Notify all players with the question (except elected)
void Server_sendASKQtoAll(Server* server){
    for(int i = 0; i < server->connectedPlayers; i++){
        Player* currentPlayer = server->players[i];
        
        // The player who asked the question is not notified
        if(currentPlayer->playerID == server->electedPlayer->playerID){
            continue;
        }

        Server_sendASKQ(server, currentPlayer);
    }
}

// Send a question to a player
void Server_sendASKQ(Server* server, Player* player){
    DataType_askq askq;
    askq.type = DATATYPE_ASKQ;    
    strcpy(askq.question, server->currentQuestion->text);

    Server_send(server, player, DATATYPE_ASKQ, &askq);

    printf("> Question sent to player #%d\n", player->playerID + 1);
}

// Notify a player about turn end
void Server_sendENDG(Server* server, Player* player){
    DataType_endg endg;
    endg.type = DATATYPE_ENDG;    
    strcpy(endg.reason, "No enough players to continue the game");

    Server_send(server, player, DATATYPE_ENDG, &endg);

    printf("> ENDG sent to #%d\n", player->playerID + 1);
}

// Set number of players
void Server_waitForPNUM(Server* server, DataType_pnum pnum){
    server->maxPlayers = pnum.numberOfPlayers;
    printf("> PNUM set to %d\n", server->maxPlayers);
}

// Retrieve question and send it to players
void Server_waitForDEFQ(Server* server, DataType_defq defq){
    server->currentQuestion = (Question*) malloc(sizeof(Question));
    strcpy(server->currentQuestion->text, defq.question);
    strcpy(server->currentQuestion->goodAnswer, defq.answer);

    printf("> DEFQ received from #%d:\n", server->electedPlayer->playerID + 1);
    printf(">  * Question: \"%s\"\n", server->currentQuestion->text);
    printf(">  * Answer: \"%s\"\n", server->currentQuestion->goodAnswer);

    Server_sendASKQtoAll(server);
}

// Check a player answer
void Server_waitForANSW(Server* server, Player* player, DataType_answ answ){
    printf("> ANSW received from #%d:\n", player->playerID+1);
    printf(">  \"%s\"\n", answ.answer);

    server->nbAnswers++;

    // Decide if the answer is correct
    int result = Server_levenshteinDistance(answ.answer,server->currentQuestion->goodAnswer);
    printf("> The answer has a distance of: %d \n",result);

    // Notify the player with the result
    if(result <= 2){
        Server_notifyANSW(server, player);
    }
    else if(server->nbAnswers >= server->connectedPlayers - 1){
        Server_notifyANSW(server, NULL);
    }
}

// Compute the levenshtein distance between player answer and question answer
int Server_levenshteinDistance (char word1[256],char word2[256]){
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = strlen(word1);
    s2len = strlen(word2);
    unsigned int column[s1len+1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x-1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (word1[y-1] == word2[x-1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    return(column[s1len]);
}
