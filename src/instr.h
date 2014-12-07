#ifndef INSTR_HEADER
#define INSTR_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "error.h"
#include "error.h"

//--- Types ---
typedef enum {
  LOAD,
  STORE,
  INTEGER,
  BRANCH,
  FPADD,
  FPMUL,
  UNKNOWN // TODO: get rid of this
} instr_operation;

typedef unsigned int reg;

typedef struct {
  instr_operation op;
  reg rs;
  reg rt;
  reg rd;
  unsigned int extra;
  
  // the address (line number) from which this instruction was loaded
  unsigned int addr;
} instr;


//--- Functions ---
error_code parse_instruction(char* str, instr** outInstr, unsigned int);

#endif
