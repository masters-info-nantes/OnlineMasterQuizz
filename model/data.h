#ifndef __DATA_H__
#define __DATA_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "datatype.h"

#define MAX_DATA_SIZE 256
#define DATA_TYPE_SIZE 4

typedef struct _Data Data;
struct _Data {
    char* type;
    void* dataType; //Structure defined in datatype.h
};

Data* Data_create(int nbParts);
Data* Data_convertFrom(DataType type, char* data);
char** Data_explode(int nbParts, char* text);

#endif