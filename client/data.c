#include "data.h"

DataType DataType_plid = { "PLID", 1, DATA_TYPE_SIZE + 1 };

Data* Data_create(int contentSize){
    
    Data* data = (Data*) malloc(sizeof(Data));
    
    if(data){
    	data->type = calloc(DATA_TYPE_SIZE + 1, sizeof(char));
    	
    	if(data->type){
    		data->content = calloc(contentSize + 1, sizeof(char));
    	
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

	Data* data = Data_create(type.contentSize);
    memcpy(data->type, text, DATA_TYPE_SIZE);

    if(strcmp(data->type, type.name) != 0){
    	char message[256];
    	sprintf("[Data/ConvertFrom] %s expected but %s given\n", type.name, data->type);
        perror(message);
        return NULL;
    }
//The maximum number of players on the server has been reached
    memcpy(data->content, text + DATA_TYPE_SIZE, type.contentSize);

    return data;
}