#ifndef INSTR_HEADER
#define INSTR_HEADER

#include "error.h"

//--- Types ---
typedef enum {
  LOAD,
  STORE,
  INTEGER,
  BRANCH,
  FPADD,
  FPMUL,
  UNKNOWN
} instr_operation;

typedef unsigned int reg;

typedef struct {
  instr_operation op;
  reg rs;
  reg rt;
  reg rd;
} instr;


//--- Functions ---
result parse_instruction(char* str, instr** outInstr);

#endif
