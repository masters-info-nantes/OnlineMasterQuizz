#include "datatype.h"

// Note: fullSize includes coma separator
DataType DataType_plid = { "PLID", 2, DATA_TYPE_SIZE + 2 };
DataType DataType_elec = { "ELEC", 2, DATA_TYPE_SIZE + 2 };
DataType DataType_defq = { "DEFQ", 0, MAX_DATA_SIZE };
DataType DataType_askq = { "ASKQ", 0, MAX_DATA_SIZE }; // 0 means variable (for question and answers)
DataType DataType_answ = { "ANSW", 2, DATA_TYPE_SIZE + 2 };
DataType DataType_resp = { "RESP", 2, DATA_TYPE_SIZE + 2 };