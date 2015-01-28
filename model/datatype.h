#ifndef __DATATYPE_H__
#define __DATATYPE_H__

typedef struct _DataType DataType;
struct _DataType
{
	char* name;
	int nbParts;
	int fullSize;
};

#include "data.h"

// Server send player identifier to client
// Form: [PLID,4]
//	 - player id: 4
extern DataType DataType_plid;

// Server elect a client for asking the next question
// Form: [ELEC,0]
//   - is elect: 0 (or 1)
extern DataType DataType_elec;

// Elected client send question to the server
// Form: [DEFQ,2+2?,3,2,5,4,34]
//	 - question: 2+2?
//   - number of answers: 3
//   - good answer: 2
//   - all answers: 5, 4, 14
extern DataType DataType_defq;

// Server send the question to all player except the elected one
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

#endif