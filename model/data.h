#ifndef __DATA_H__
#define __DATA_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_DATA_SIZE 256
#define DATA_TYPE_SIZE 4

typedef struct _Data Data;
struct _Data {
    char* type;
    char** content;
};

typedef struct _DataType DataType;
struct _DataType
{
	char* name;
	int nbParts;
	int fullSize;
};

// Server send player identifier to client
// Form: [PLID,4]
//	 - player id: 4
extern DataType DataType_plid;

// Server elect a client for asking the next question
// Form: [ELEC,0]
//   - is elect: 0 (or 1)
extern DataType DataType_elec;

// Elected client send question to the server
// Form: [ASKQ,2+2?,3,2,5,4,34]
//	 - question: 2+2?
//   - number of answers: 3
//   - good answer: 2
//   - all answers: 5, 4, 14
extern DataType DataType_askq;

// Client answer to the server question
// Form: [ANSW,2]
//   - answer id: 2
extern DataType DataType_answ;

// Server send response to all clients
// Form: [RESP,2]
//   - good answer: 2
extern DataType DataType_resp;

Data* Data_create(int nbParts);
Data* Data_convertFrom(DataType type, char* data);
char** Data_explode(int nbParts, char* text);

#endif