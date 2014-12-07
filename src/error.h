#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>
#include <assert.h>

#define SUCCESS 1
#define UNKNOWN_ERROR 0
#define INSTR_PARSING_ERROR -1
#define IDX_OVERFLOW -2
#define BACKEND_FINISHED -3;
#define INSTR_QUEUE_FULL -4;

typedef int error_code;

//--- Functions ---
const char* get_error(error_code code);

#endif
