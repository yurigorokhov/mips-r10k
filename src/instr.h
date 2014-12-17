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
  NONE,
  FETCH,
  DECODE,
  ISSUE,
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
  
  // for book keeping only
  unsigned int step;
  instr_stage stage;
} instr;


//--- Functions ---
error_code parse_instruction(char* str, instr** outInstr, unsigned int);
const char* get_str(instr_stage stage, instr_operation);
char* instr_str(instr* instruction, char** str);

#endif
