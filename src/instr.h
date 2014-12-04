#ifndef INSTR_HEADER
#define INSTR_HEADER

//--- Types ---
typedef enum {
  LOAD,
  STORE,
  INTEGER,
  BRANCH,
  FPADD,
  FPMUL  
} instr_operation;

typedef struct {
  instr_operation op;
} instr;

//--- Functions ---
instr parse_instruction(const char* str);

#endif
