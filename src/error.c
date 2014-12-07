#include "error.h"

const char* get_error(error_code code) {
  assert(code != SUCCESS);
  switch(code) {
  case INSTR_PARSING_ERROR: return "there was an error parsing the instruction";
  case UNKNOWN_ERROR:
  default: return "an unknown error occured";
  }
};
