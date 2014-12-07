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

  // TODO: more may be needed
  NONE,
  FETCH,
  EXECUTE,
  FINISHED,
  COMMITTED
} instr_stage;

typedef enum {
  LOAD,
  STORE,
  INTEGER,
  BRANCH,
  FPADD,
  FPMUL,
  UNKNOWN // only possible at parsing time (will generate error)
} instr_operation;

typedef unsigned int reg;

typedef struct {
  instr_operation op;
  reg rs; // source 1
  reg rt; // source 2
  reg rd; // destination
  unsigned int extra;
  
  // the address (line number) from which this instruction was loaded
  unsigned int addr;
  instr_stage stage;
} instr;


//--- Functions ---
error_code parse_instruction(char* str, instr** outInstr, unsigned int);

#endif
