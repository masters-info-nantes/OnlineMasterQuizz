#ifndef __DATA_H__
#define __DATA_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DATA_TYPE_SIZE 4

typedef struct _Data Data;
struct _Data {
    char* type;
    char* content;
};

typedef struct _DataType DataType;
struct _DataType
{
	char* name;
	int contentSize;
	int fullSize;
};

extern DataType DataType_plid;

Data* Data_create(int contentSize);
Data* Data_convertFrom(DataType type, char* data);

#endif