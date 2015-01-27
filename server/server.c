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
  
    // Wait for new incoming connexions
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
    player->networkDetails = clientInfos;

    // Create thread dedicated to the new client
    int threadCreated = pthread_create(&server->clientsThread[server->connectedPlayers], 
                                       NULL, Player_clientThread,
                                       (void*)player
    );
    if(threadCreated){
        perror("[Server/AddPlayer] Cannot create thread for new client\n");
        exit(1);
    }

    server->players[server->connectedPlayers] = player;
    server->connectedPlayers++;

    Player_printClientInfos(player);  
}
