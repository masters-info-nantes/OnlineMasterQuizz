#ifndef __QUESTION_H__
#define __QUESTION_H__

#include <stdlib.h>

typedef struct _Question Question;
struct _Question{
	char* text;
	char* goodAnswer;
};

#endif