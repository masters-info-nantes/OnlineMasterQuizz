#include "client.h"

Client* Client_create(){

	Client* client = (Client*)malloc(sizeof(Client));

	if(client){
		client->socketID = -1;
        client->socketInfos = (sockaddr_in*) malloc(sizeof(sockaddr_in));
        client->elected=false;
        if(client->socketInfos){
            client->clientThread = (pthread_t*) malloc(sizeof(pthread_t));
            if(client->clientThread){

                client->answerThread = (pthread_t*) malloc(sizeof(pthread_t));
                if(client->socketInfos == NULL){   
                    free(client->socketInfos);             
                    free(client->clientThread);
                    free(client);
                }
            }
            else {
                free(client->socketInfos);
                free(client);
            }
        }
        else {
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
    
    Client_runReceiveThread(client);
    return true;
}

void Client_runReceiveThread(Client* client){
    void** threadParams = (void**) calloc(1, sizeof(void*));
    threadParams[0] = client;

    // Create thread dedicated to the new client
    int threadCreated = pthread_create(client->clientThread, 
                                       NULL, Client_threadReceive,
                                       (void*)threadParams
    );
    
    if(threadCreated){
        perror("[Client/RunReceiveThread] Cannot create thread for new client\n");
        exit(EXIT_FAILURE);
    }
}

void Client_send(Client* client, int type, void* data){

    // First trame: notify data type
    DataType typeNotif = { type };
    if(write(client->socketID, &typeNotif, sizeof(typeNotif)) <= 0){
        perror("[Client/send] Cannot send TYPE to server");
        exit(EXIT_FAILURE);
    }

    // Second trame: data
    switch(typeNotif.type){
        case DATATYPE_PNUM: {
            DataType_pnum pnum = *((DataType_pnum*)data);

            if(write(client->socketID, &pnum, sizeof(pnum)) <= 0){
                perror("[Client/send] Cannot send PNUM to server");
                exit(EXIT_FAILURE);
            }
        }
        break;   

         case DATATYPE_DEFQ: {
            DataType_defq defq = *((DataType_defq*)data);

            if(write(client->socketID, &defq, sizeof(defq)) <= 0){
                perror("[Client/send] Cannot send DEFQ to server");
                exit(EXIT_FAILURE);
            }
        }
        break; 

        case DATATYPE_ANSW: {
            DataType_answ answ = *((DataType_answ*)data);

            if(write(client->socketID, &answ, sizeof(answ)) <= 0){
                perror("[Client/send] Cannot send ANSW to server");
                exit(EXIT_FAILURE);
            }
        }
        break;                                                                
    }    
}

void Client_receive(Client* client){
    DataType typeNotif;

    if(read(client->socketID, &typeNotif, sizeof(typeNotif)) > 0){
        switch(typeNotif.type){
            case DATATYPE_PLID: {
                DataType_plid plid;
                if(read(client->socketID, &plid, sizeof(plid)) > 0){
                    Client_waitForPLID(client, plid);
                }
            }
            break;

            case DATATYPE_PNUM: {
                DataType_pnum pnum;
                if(read(client->socketID, &pnum, sizeof(pnum)) > 0){
                    Client_waitForPNUM(client, pnum);
                }
            }
            break;

            case DATATYPE_ELEC: {
                DataType_elec elec;
                if(read(client->socketID, &elec, sizeof(elec)) > 0){
                    Client_waitForELEC(client, elec);
                }
            }
            break;

            case DATATYPE_ASKQ: {
                DataType_askq askq;
                if(read(client->socketID, &askq, sizeof(askq)) > 0){
                    Client_waitForASKQ(client, askq);
                }
            }
            break;

            case DATATYPE_RESP: {
                DataType_resp resp;
                if(read(client->socketID, &resp, sizeof(resp)) > 0){
                    Client_waitForRESP(client, resp);
                }
            }
            break;   

            case DATATYPE_ENDG: {
                DataType_endg endg;
                if(read(client->socketID, &endg, sizeof(endg)) > 0){
                    Client_waitForENDG(client, endg);
                }
            }
            break;                                                         
        }        
    }     
}

void* Client_threadReceive(void* params){
    void** paramList = (void**) params;
    Client* client = (Client*) paramList[0];

    while(true){
        Client_receive(client);
    }

    free(params);
    return NULL;
}

/***************************  Function for each request type ****************************/
void Client_waitForPLID(Client* client, DataType_plid plid){
    printf("PlayerID %u\n", plid.playerId); 
}

void Client_waitForPNUM(Client* client, DataType_pnum pnum){
    if(pnum.numberOfPlayers == 1)
    {
        printf("You are the first player! \n");
        printf("How many players do you want for this game? (2 to 10) \n");

        int number = 0;
        char numtext[256];

        while(number<2||number>10)
        {
            fgets(numtext,sizeof(numtext),stdin);
            number = atoi(numtext);
        }
        Client_sendPNUM(client,number);
    }
    printf("Waiting for other players...\n");
}

void Client_sendPNUM(Client* client, int playerCount)
{
    DataType_pnum pnum = { DATATYPE_PNUM, playerCount };
    Client_send(client, DATATYPE_PNUM, &pnum);    
}

void Client_sendDEFQ(Client* client, Question* question){
    DataType_defq defq;

    defq.type = DATATYPE_DEFQ;
    strcpy(defq.question,question->text);
    strcpy(defq.answer,question->goodAnswer);

    Client_send(client, DATATYPE_DEFQ, &defq);  
    client->elected=true;
    printf("Your question has been sent! \n");
}

void Client_sendANSW(Client* client, char* answer)
{
    DataType_answ answ;
    answ.type = DATATYPE_ANSW;
    strcpy(answ.answer,answer);

    Client_send(client, DATATYPE_ANSW, &answ);    
    printf("Answer sent!\n");
}

void Client_waitForELEC(Client* client, DataType_elec elec){
    if(elec.elected==1)
    {
        printf("You are elected to choose the question! \n");
        printf("What's your question? \n");
        Question question;
        fgets(question.text,sizeof(question.text),stdin);
        printf("What's the correct answer? \n");
        fgets(question.goodAnswer,sizeof(question.goodAnswer),stdin);
        Client_sendDEFQ(client,&question);
    }    
    else
    {
        printf("Waiting for a question...\n");
    }
}

void* Client_threadAnswer(void* params)
{
    void** paramList = (void**) params;
    Client* client = (Client*) paramList[0];   
    printf("Your Answer: ");
    char answer[256];
    fgets(answer,sizeof(answer),stdin);
    Client_sendANSW(client,answer);
    printf("Waiting for other players to answer...\n");

    free(params);
    return NULL;
}

void Client_waitForASKQ(Client* client, DataType_askq askq){
    printf("Question: %s \n",askq.question);

    void** threadParams = (void**) calloc(1, sizeof(void*));
    threadParams[0] = client;

    // Création de thread
    int threadCreated = pthread_create(client->answerThread, 
                                       NULL, Client_threadAnswer,
                                       (void*)threadParams
    );
     
    if(threadCreated){
        perror("[Client/RunReceiveThread] Cannot create thread for new client\n");
        exit(EXIT_FAILURE);
    }
}

void Client_waitForRESP(Client* client, DataType_resp resp){
    pthread_cancel(*client->answerThread);
    if(!client->elected)
    {
        printf("Good answer: %s\n",resp.answer);
        printf("Your points: %d\n",resp.score);
    }
    else
    {
        printf("The turn is over!\n");
        client->elected=false;
    }
}

void Client_waitForENDG(Client* client, DataType_endg endg){
    printf("Game over, reason: %s\n", endg.reason);
}
