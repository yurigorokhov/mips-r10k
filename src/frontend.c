#include "frontend.h"

static instr** instruction_buffer;
static unsigned int idx = 0;
static unsigned int capacity = 0;

void frontend_init(unsigned int size) {
  instruction_buffer = (instr**)malloc(size*sizeof(instr*));
  capacity = size;
}

void frontend_enqueue(instr* instruction) {
  if(idx == capacity - 1) {
    instruction_buffer = (instr**)realloc(instruction_buffer, 
					  100+(capacity*sizeof(instr*)));
    capacity += 100;
  }
  instruction_buffer[idx++] = instruction;
}

void frontend_clean() {
  unsigned int i;
  for(i = 0; i < idx; ++i) {
    free(instruction_buffer[i]);
  }
  free(instruction_buffer);
}

error_code frontend_getinstr(unsigned int addr, instr** res) {
  if(addr > idx || addr < 1) {
    return IDX_OVERFLOW;
  }
  *res = instruction_buffer[addr-1];
  return SUCCESS;
}

unsigned int frontend_get_instr_count() {
  return idx;
}
