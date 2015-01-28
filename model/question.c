#include "question.h"

Question* Question_create(int nbAnswers){

	Question* question = (Question*) malloc(sizeof(Question));

	if(question){
		question->nbAnswers = nbAnswers;
		question->allAnswers = (char**) calloc(nbAnswers, sizeof(char*));
		
		if(question->allAnswers == NULL){
			free(question);
		}
	}

	return question;
}