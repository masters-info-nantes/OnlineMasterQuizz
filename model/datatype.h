#ifndef __DATATYPE_H__
#define __DATATYPE_H__

#define MAX_CHAR_LENGTH 256

#define DATATYPE_PLID 0
#define DATATYPE_PNUM 1
#define DATATYPE_ELEC 2
#define DATATYPE_DEFQ 3
#define DATATYPE_ASKQ 4
#define DATATYPE_ANSW 5
#define DATATYPE_RESP 6
#define DATATYPE_ENDG 7 // Game end

typedef struct _DataType DataType;
struct _DataType {
	unsigned int type;
};

// Server send player identifier to client
//	 - player id: 4
typedef struct _DataType_plid DataType_plid;
struct _DataType_plid
{
   unsigned int type;
   unsigned int playerId;  
};

// Server asks player the number of players (if=1, clients answers with numberOfPlayers)
// Player choose the players number
// - players number: 4
typedef struct _DataType_pnum DataType_pnum;
struct _DataType_pnum
{
   unsigned int type;	
   unsigned int numberOfPlayers;  
};

// Server elect a client for asking the next question
//   - is elected: 0 (or 1)
typedef struct _DataType_elec DataType_elec;
struct _DataType_elec
{
   unsigned int type;	
   bool elected;  
};

// Elected client send question to the server
//	 - question: "2+2?="
//   - good answer: "4"
typedef struct _DataType_defq DataType_defq;
struct _DataType_defq
{
   unsigned int type;	
   char question[MAX_CHAR_LENGTH];
   char answer[MAX_CHAR_LENGTH];
};

// Server send the question to all player except the elected one
//	 - question: "2+2?"
typedef struct _DataType_askq DataType_askq;
struct _DataType_askq
{
   unsigned int type;	
   char question[MAX_CHAR_LENGTH];
};

// Client answer to the server question
//   - answer: "2"
typedef struct _DataType_answ DataType_answ;
struct _DataType_answ
{
   unsigned int type;	
   char answer[MAX_CHAR_LENGTH]; 
};

// Server send response to all clients
//   - score: 100
//   - good answer: 2
typedef struct _DataType_resp DataType_resp;
struct _DataType_resp
{
   unsigned int type;	
   char answer[MAX_CHAR_LENGTH];
   unsigned int score;
};

typedef struct _DataType_endg DataType_endg;
struct _DataType_endg
{
   unsigned int type;
   char reason[MAX_CHAR_LENGTH];
};

#endif