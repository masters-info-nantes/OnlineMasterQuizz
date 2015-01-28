#ifndef __QUESTION_H__
#define __QUESTION_H__

#include <stdlib.h>

typedef struct _Question Question;
struct _Question{
	char* title;
	int nbAnswers;
	char* goodAnswer; // Id from 0
	char** allAnswers;
};

Question* Question_create(int nbAnswers);

#endif