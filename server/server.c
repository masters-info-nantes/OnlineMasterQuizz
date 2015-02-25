#include "server.h"

Server* Server_create(){
	Server* server = (Server*) malloc(sizeof(Server));
	
	if(server){
		server->socketID = -1;
		server->connectedPlayers = 0;
        server->maxPlayers = 1;
        server->electedPlayer = NULL;

		server->players = calloc(MAX_PLAYERS, sizeof(Player*));
		if(server->players == NULL){			
			free(server);
		}
	}

    srand(time(NULL)); // For real random
	
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

    while(server->connectedPlayers < server->maxPlayers) {
    
        clientInfosSize = sizeof(clientInfos);
		int socketID = accept(server->socketID, (sockaddr*)clientInfos, &clientInfosSize);

		if (socketID < 0) {
		    perror("[Server/WaitForClients] Cannot initiate connexion with a new client\n");
		    exit(1);
		}

		Server_addPlayer(server, socketID, clientInfos);

        // Wait for PNUM from the first player
        if(server->connectedPlayers == 1){
            while(server->maxPlayers == 1);
        }
    } 
    
    printf("> No places left for this turn !\n\n");
    Server_electPlayer(server);
}

void Server_addPlayer(Server* server, int socketID, sockaddr_in* clientInfos){

    Player* player = Player_create();
    if(player == NULL){
    	return;
    }

    player->socketID = socketID;
    player->playerID = server->connectedPlayers; 
    player->socketInfos = clientInfos;

    void** threadParams = (void**) calloc(2, sizeof(void*));
    threadParams[0] = player;
    threadParams[1] = server;

    // Create thread dedicated to the new client
    int threadCreated = pthread_create(player->waitingThread, 
                                       NULL, Player_receive,
                                       (void*)threadParams
    );

    if(threadCreated){
        perror("[Server/AddPlayer] Cannot create thread for new client\n");
        exit(1);
    }

    printf("\n[Server/WaitForClients] New client connected #%d\n", server->connectedPlayers); 
    Player_printClientInfos(player);  

    server->players[server->connectedPlayers] = player;
    server->connectedPlayers++;

    Server_sendPLID(server, player);

    bool isFirstClient = (player->playerID == 0) ? true : false;
    Server_sendPNUM(server, player, isFirstClient);
}

void Server_electPlayer(Server* server){
    int electedID = rand() % (server->connectedPlayers - 1);

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

void Server_notifyGoodANSW(Server* server, Player* player){
    printf("> Player #%d has the good answer\n", player->playerID);

    for(int i = 0; i < server->connectedPlayers; i++){
        Player* currentPlayer = server->players[i];
        player->score += (currentPlayer->playerID == player->playerID) ? 1 : 0;
        Server_sendRESP(server, currentPlayer);
    }
}

void Server_send(Server* server, Player* player, int type, void* data){

    // First trame: notify data type
    DataType typeNotif = { type };
    if(write(player->socketID, &typeNotif, sizeof(typeNotif)) <= 0){
        char message[500];
        sprintf(message, "[Server/send] Cannot send type notif to #%d", player->playerID + 1);
        perror(message);
        exit(0);
    }

    // Second trame: data
    switch(typeNotif.type){
        case DATATYPE_PLID: {
            DataType_plid plid = *((DataType_plid*)data);

            if(write(player->socketID, &plid, sizeof(plid)) <= 0){
                char message[500];
                sprintf(message, "[Server/send] Cannot send data to #%d", player->playerID + 1);
                perror(message);
                exit(0);
            }
        }
        break;

        case DATATYPE_PNUM: {
            DataType_pnum pnum = *((DataType_pnum*)data);

            if(write(player->socketID, &pnum, sizeof(pnum)) <= 0){
                char message[500];
                sprintf(message, "[Server/send] Cannot send data to #%d", player->playerID + 1);
                perror(message);
                exit(0);
            }
        }
        break; 

        case DATATYPE_ELEC: {
            DataType_elec elec = *((DataType_elec*)data);

            if(write(player->socketID, &elec, sizeof(elec)) <= 0){
                char message[500];
                sprintf(message, "[Server/send] Cannot send data to #%d", player->playerID + 1);
                perror(message);
                exit(0);
            }
        }
        break;

        case DATATYPE_ASKQ: {
            DataType_askq askq = *((DataType_askq*)data);

            if(write(player->socketID, &askq, sizeof(askq)) <= 0){
                char message[500];
                sprintf(message, "[Server/send] Cannot send data to #%d", player->playerID + 1);
                perror(message);
                exit(0);
            }
        }
        break;

        case DATATYPE_RESP: {
            DataType_resp resp = *((DataType_resp*)data);

            if(write(player->socketID, &resp, sizeof(resp)) <= 0){
                char message[500];
                sprintf(message, "[Server/send] Cannot send data to #%d", player->playerID + 1);
                perror(message);
                exit(0);
            }
        }
        break;                                                           
    }  
}

void Server_receive(Server* server, Player* player){
    DataType typeNotif;

    if(read(player->socketID, &typeNotif, sizeof(typeNotif)) > 0){
        switch(typeNotif.type){

            case DATATYPE_PNUM: {
                DataType_pnum pnum;
                if(read(player->socketID, &pnum, sizeof(pnum)) > 0){
                    Server_waitForPNUM(server, pnum);
                }
            }
            break;

            case DATATYPE_DEFQ: {
                DataType_defq defq;
                if(read(player->socketID, &defq, sizeof(defq)) > 0){
                    Server_waitForDEFQ(server, defq);                   
                }
            }
            break;

            case DATATYPE_ANSW: {
                DataType_answ answ;
                if(read(player->socketID, &answ, sizeof(answ)) > 0){
                    Server_waitForANSW(server, player, answ);
                }
            }
            break;                                                
        }        
    } 
}

void Server_waitForGoodAnswers(Server* server){
    for(int i = 0; i < server->connectedPlayers; i++){
        pthread_join(*(server->players[i]->waitingThread), NULL);
    }
}

/***************************  Function for each request type ****************************/
void Server_sendPLID(Server* server, Player* player){
    DataType_plid plid = { DATATYPE_PLID, player->playerID + 1 }; 
    Server_send(server, player, DATATYPE_PLID, &plid);     
    printf("> PLID sent to player #%d\n", player->playerID + 1);
}

void Server_sendPNUM(Server* server, Player* player, bool allowed){
    DataType_pnum pnum = { DATATYPE_PNUM, allowed};
    Server_send(server, player, DATATYPE_PNUM, &pnum);     
    printf("> PNUM %s sent to player #%d\n", (pnum.numberOfPlayers == 1) ? "authorization" : "not authorized", player->playerID + 1);
}

void Server_sendELEC(Server* server, Player* player, bool elected)
{    
    DataType_elec elec = { DATATYPE_ELEC, elected };
    Server_send(server, player, DATATYPE_ELEC, &elec);
    printf("> Player #%d has%s been elected to ask the question\n", player->playerID + 1, (elected) ? "" : " not");
}

void Server_sendRESP(Server* server, Player* player){
    DataType_resp resp = { DATATYPE_RESP, server->currentQuestion->goodAnswer, player->score };
    Server_send(server, player, DATATYPE_RESP, &resp);
    printf("> Send response to player #%d (has %d score)\n", player->playerID + 1, player->score);
}

void Server_sendASKQtoAll(Server* server){
    for(int i = 0; i < server->connectedPlayers; i++){
        Player* currentPlayer = server->players[i];
        
        if(currentPlayer->playerID == server->electedPlayer->playerID){
            continue;
        }

        Server_sendASKQ(server, currentPlayer);
    }
}

void Server_sendASKQ(Server* server, Player* player){
    DataType_askq askq;
    askq.type = DATATYPE_ASKQ;    
    strcpy(askq.question, server->currentQuestion->text);

    Server_send(server, player, DATATYPE_ASKQ, &askq);

    printf("> Question sent to player #%d\n", player->playerID + 1);
}

void Server_waitForPNUM(Server* server, DataType_pnum pnum){
    server->maxPlayers = pnum.numberOfPlayers;
    printf("> PNUM set to %d\n", server->maxPlayers);
}

void Server_waitForDEFQ(Server* server, DataType_defq defq){
    server->currentQuestion = (Question*) malloc(sizeof(Question));
    strcpy(server->currentQuestion->text, defq.question);
    strcpy(server->currentQuestion->goodAnswer, defq.answer);

    printf("> DEFQ received from #%d:\n", server->electedPlayer->playerID + 1);
    printf(">  * Question: \"%s\"\n", server->currentQuestion->text);
    printf(">  * Answer: \"%s\"\n", server->currentQuestion->goodAnswer);

    Server_sendASKQtoAll(server);
}

void Server_waitForANSW(Server* server, Player* player, DataType_answ answ){
    printf("> ANSW received from #%d:\n", player->playerID+1);
    printf(">  \"%s\"\n", answ.answer);

    int result = Server_levenshteinDistance(answ.answer,server->currentQuestion->goodAnswer);
    printf("> The answer has a distance of: %d \n",result);
    if(result <= 2)
    {
        Server_notifyGoodANSW(server,player);
    }
}

int Server_levenshteinDistance (char word1[256],char word2[256])
{
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
