#include "client.h"

Client* Client_create(){

	Client* client = (Client*)malloc(sizeof(Client));

	if(client){
		client->socketID = -1;
        client->socketInfos = (sockaddr_in*) malloc(sizeof(sockaddr_in));

        if(client->socketInfos == NULL){
            free(client);
        }
	}

	return client;
}

bool Client_run(Client* client, char* serverName, int serverPort){

    hostent* serverInfos = gethostbyname(serverName);
    if (serverInfos == NULL) {
       perror("[Client/Run] Cannot find server from given hostname\n");
       return false;
    }

    // Fill socket infos with server infos
    bcopy((char*)serverInfos->h_addr, (char*)&client->socketInfos->sin_addr, serverInfos->h_length);
    client->socketInfos->sin_family = AF_INET;
    client->socketInfos->sin_port = htons(serverPort);

    // Socket configuration and creation
    printf("[Client/Run] Connect to server %s:%d\n", serverInfos->h_name, ntohs(client->socketInfos->sin_port));
        
    client->socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (client->socketID < 0) {
        perror("[Client/Run] Unable to create socket connexion\n");
        return false;
    }
    
    if ((connect(client->socketID, (sockaddr*)(client->socketInfos), sizeof(sockaddr))) < 0) {
        perror("[Client/Run] Cannot connect to the server\n");
        return false;
    }
    printf("[Client/Run] Connection sucessfull\n\n");
    
    return true;
}

void Client_waitForPLID(Client* client){
    DataType_plid plid;

    if(read(client->socketID, &plid, sizeof(plid)) > 0){
        printf("PlayerID %u\n", plid.playerId); 
        Client_waitForPNUM(client);
    }  
}

void Client_waitForPNUM(Client* client){
    DataType_pnum pnum;

    if(read(client->socketID, &pnum, sizeof(pnum)) > 0){
        if(pnum.numberOfPlayers==1)
        {
            printf("You are the first player! \n");
            printf("How many players do you want for this game? \n");
            int number;
            scanf("%d",&number);
            Client_sendPNUM(client,number);
        }
        else
        {
            Client_waitForELEC(client);
        }     
    }  
}

void Client_sendPNUM(Client* client, int playerCount)
{
    DataType_pnum pnum;
    pnum.numberOfPlayers=playerCount;
    if ((write(client->socketID, &pnum, sizeof(pnum))) > 0) {
        printf("number of players sent \n");
        printf("waiting for other players to connect... \n");
        Client_waitForELEC(client);
    }
    
}
void Client_sendDEFQ(Client* client, Question* question){
    DataType_defq defq;
    //defq.question = question->text;
    strcpy(defq.question,question->text);
    strcpy(defq.answer,question->goodAnswer);
    //defq.answer = question->goodAnswer;
    if ((write(client->socketID, &defq, sizeof(defq))) > 0) {
        printf("Your question has been sent! \n");
        Client_waitForRESP(client);
    }
}

void Client_sendANSW(Client* client, char answer[256])
{
    DataType_answ answ;
    strcpy(answ.answer,answer);
    if ((write(client->socketID, &answ, sizeof(answ))) > 0) {
            printf("Answer sent!\n");
        Client_waitForRESP(client);
    }
}

void Client_waitForELEC(Client* client){
    DataType_elec elec;

    if(read(client->socketID, &elec, sizeof(elec)) > 0){
        if(elec.elected==1)
        {
            printf("You are elected to choose the question! \n");
            printf("What's your question? \n");
            Question question;
            scanf("%s",question.text);
            printf("What's the correct answer? \n");
            scanf("%s",question.goodAnswer);
            Client_sendDEFQ(client,&question);
        }
        else
        {
            Client_waitForASKQ(client);
        }     
    } 
}

void Client_waitForASKQ(Client* client){
    printf("Wait for question...\n");
    DataType_askq askq;

    if(read(client->socketID, &askq, sizeof(askq)) > 0){
        printf("Question: %s \n",askq.question);
        printf("Your Answer: ");
        char answer[256];
        scanf("%s",answer);
        Client_sendANSW(client,answer);
    }
}

void Client_waitForRESP(Client* client){
    printf("Waiting for other players to answer...\n");
    DataType_resp resp;

    if(read(client->socketID, &resp, sizeof(resp)) > 0){
        printf("Good answer: %s \n",resp.answer);
        printf("Your points: %d \n",resp.score);
        Client_waitForELEC(client);
    }
}