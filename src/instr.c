#include "instr.h"

error_code sm_parse_op(char *, instr_operation*);
instr_operation parse_op(char);
void skip_whitespace(char**);
error_code sm_parse_next_hex(char**, unsigned int*);

//--- Public Functions ---
error_code parse_instruction(char* str, instr** res, unsigned int addr) {
  char* str_ptr = str;
  
  // Parse operation
  instr_operation op;
  error_code code = sm_parse_op(str_ptr, &op);
  if(SUCCESS != code) {
      return code;
  }
  if(UNKNOWN == op) {
    return INSTR_PARSING_ERROR;
  }
  str_ptr++;

  // parse rs
  reg rs;
  code = sm_parse_next_hex(&str_ptr, &rs);
  if(SUCCESS != code) {
    return code;
  }
  
  // parse rt
  reg rt;
  code = sm_parse_next_hex(&str_ptr, &rt);
  if(SUCCESS != code) {
    return code;
  }

  // parse rd
  reg rd;
  code = sm_parse_next_hex(&str_ptr, &rd);
  if(SUCCESS != code) {
    
    // rd is optional for some instructions
    rd = UINT_MAX;
  }

  unsigned int extra;
  code = sm_parse_next_hex(&str_ptr, &extra);
  if(SUCCESS != code) {
    
    // extra is optional for some instructions
    extra = UINT_MAX;
  }

  // return result
  *res = (instr*)malloc(sizeof(instr));
  (*res)->op = op;
  (*res)->rs = rs;
  (*res)->rt = rt;
  (*res)->rd = rd;
  (*res)->extra = rd;
  (*res)->addr = addr;
  return SUCCESS;
}

//--- Private Functions ---
void skip_whitespace(char **str) {
  while(' ' == **str) { ++(*str); }
}

error_code sm_parse_op(char* str, instr_operation *op) {
  skip_whitespace(&str);
  if('\0' == *str) { return INSTR_PARSING_ERROR; }
  *op = parse_op(*str);
  return SUCCESS;
}

error_code sm_parse_next_hex(char** c, unsigned int* r) {
  
  // find the last char
  skip_whitespace(&(*c));
  char regStr[10];
  regStr[0] = '0';
  regStr[1] = 'x';
  unsigned short idx = 2;
  while(' ' != (**c) && '\0' != (**c)) {
    if(idx > 8) {
      return IDX_OVERFLOW;
    }
    regStr[idx] = **c;
    (*c)++; idx++;
  };
  if(idx == 2) {
    return INSTR_PARSING_ERROR;
  }
  regStr[idx] = '\0';
  int regNum;
  sscanf(regStr, "%x", &regNum);
  *r = regNum;
  return SUCCESS;
}

instr_operation parse_op(char c) {
  switch(c) {
  case 'l':
  case 'L':
    return LOAD;
  case 's':
  case 'S':
    return STORE;
  case 'i':
  case 'I':
    return INTEGER;
  case 'b':
  case 'B':
    return BRANCH;
  case 'a':
  case 'A':
    return FPADD;
  case 'm':
  case 'M':
    return FPMUL;
  default: 
    return UNKNOWN;
  }
}
