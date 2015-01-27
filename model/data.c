#include "data.h"

// Note: fullSize includes coma separator
DataType DataType_plid = { "PLID", 2, DATA_TYPE_SIZE + 2 };
DataType DataType_elec = { "ELEC", 2, DATA_TYPE_SIZE + 2 };
DataType DataType_askq = { "ASKQ", 2, DATA_TYPE_SIZE + 2 };
DataType DataType_answ = { "ANSW", 0, MAX_DATA_SIZE - DATA_TYPE_SIZE }; // 0 to calcul
DataType DataType_resp = { "RESP", 2, DATA_TYPE_SIZE + 2 };

Data* Data_create(int nbParts){
    
    Data* data = (Data*) malloc(sizeof(Data));
    
    if(data){
    	data->type = calloc(DATA_TYPE_SIZE + 1, sizeof(char));
    	
    	if(data->type){
    		data->content = calloc(nbParts, sizeof(char*));
    	
    		if(data->content == NULL){
    			free(data->type);
    			free(data);
    		}
    	}
    	else {
    		free(data);
    	}
    }

    return data;	
}

Data* Data_convertFrom(DataType type, char* text){

	Data* data = Data_create(type.nbParts);
    char** content = Data_explode(type.nbParts, text);

    data->type = content[0];

    if(strcmp(data->type, type.name) != 0){
    	char message[256];
    	sprintf("[Data/ConvertFrom] %s expected but %s given\n", type.name, data->type);
        perror(message);
        return NULL;
    }

    for(int i = 0; i < type.nbParts - 1; i++){
        data->content[i] = content[i + 1];
    }

    return data;
}

char** Data_explode(int nbParts, char* text){
    char** content = (char**) calloc(nbParts, sizeof(char*));
    int currentPart = 0;

    char* part;
    part = strtok (text, ",");

    while (part != NULL){
        content[currentPart] = part;

        currentPart++;
        part = strtok (NULL, ",");
    }

    return content;
}
