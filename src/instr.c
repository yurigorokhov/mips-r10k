#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "instr.h"
#include "error.h"

result sm_parse_op(char *, instr_operation*);
instr_operation parse_op(char);
void skip_whitespace(char**);
result sm_parse_next_reg(char**, reg*);

//--- Public Functions ---
result parse_instruction(char* str, instr** res) {
  char* str_ptr = str;
  
  // Parse operation
  instr_operation op;
  result parseRes = sm_parse_op(str_ptr, &op);
  if(ERROR == parseRes.status) {
      return parseRes;
  }
  if(UNKNOWN == op) {
    return get_error("The operation is unknown");
  }
  str_ptr++;

  // parse rs
  reg rs;
  parseRes = sm_parse_next_reg(&str_ptr, &rs);
  if(ERROR == parseRes.status) {
    return get_error("Could not parse rs");
  }
  
  // parse rt
  reg rt;
  parseRes = sm_parse_next_reg(&str_ptr, &rt);
  if(ERROR == parseRes.status) {
    return get_error("Could not parse rt");
  }

  // parse rd
  reg rd;
  parseRes = sm_parse_next_reg(&str_ptr, &rd);
  if(ERROR == parseRes.status) {
    return get_error("Could not parse rd");
  }

  // return result
  *res = (instr*)malloc(sizeof(instr));
  (*res)->op = op;
  (*res)->rs = rs;
  (*res)->rt = rt;
  (*res)->rd = rd;
  return get_success();
}

//--- Private Functions ---
void skip_whitespace(char **str) {
  while(' ' == **str) { ++(*str); }
}

result sm_parse_op(char* str, instr_operation *op) {
  skip_whitespace(&str);
  if('\0' == *str) { return get_error("reached EOL before finding operation"); }
  *op = parse_op(*str);
  return get_success();
}

result sm_parse_next_reg(char** c, reg* r) {
  
  // find the last char
  skip_whitespace(&(*c));
  char regStr[20];
  regStr[0] = '0';
  regStr[1] = 'x';
  unsigned short idx = 2;

  //TODO: make sure we don't overflow
  while(' ' != (**c) && '\0' != (**c)) {
    regStr[idx] = **c;
    (*c)++; idx++;
  };
  if(idx == 2) {
    return get_error("could not find another register");
  }
  regStr[idx] = '\0';
  int regNum;
  sscanf(regStr, "%x", &regNum);
  *r = regNum;
  return get_success();
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
