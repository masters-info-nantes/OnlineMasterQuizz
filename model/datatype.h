#ifndef __DATATYPE_H__
#define __DATATYPE_H__

#define MAX_CHAR_LENGTH 256

// Server send player identifier to client
//	 - player id: 4
typedef struct _DataType_plid DataType_plid;
struct _DataType_plid
{
   unsigned int playerId;  
};

// Server asks player the number of players (if=1, clients answers with numberOfPlayers)
// Player choose the players number
// - players number: 4
typedef struct _DataType_pnum DataType_pnum;
struct _DataType_pnum
{
   unsigned int numberOfPlayers;  
};

// Server elect a client for asking the next question
//   - is elected: 0 (or 1)
typedef struct _DataType_elec DataType_elec;
struct _DataType_elec
{
   bool elected;  
};

// Elected client send question to the server
//	 - question: "2+2?="
//   - good answer: "4"
typedef struct _DataType_defq DataType_defq;
struct _DataType_defq
{
   char question[MAX_CHAR_LENGTH];
   char answer[MAX_CHAR_LENGTH];
};

// Server send the question to all player except the elected one
//	 - question: "2+2?"
typedef struct _DataType_askq DataType_askq;
struct _DataType_askq
{
   char question[MAX_CHAR_LENGTH];
};

// Client answer to the server question
//   - answer: "2"
typedef struct _DataType_answ DataType_answ;
struct _DataType_answ
{
   char answer[MAX_CHAR_LENGTH]; 
};

// Server send response to all clients
//   - score: 100
//   - good answer: 2
typedef struct _DataType_resp DataType_resp;
struct _DataType_resp
{
   char answer[MAX_CHAR_LENGTH];
   unsigned int score;
};

#endif