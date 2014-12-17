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

error_code frontend_getinstr_with_step(unsigned int step, instr** res) {
  unsigned int i;
  for(i = 0; i < idx; ++i) {
    if(instruction_buffer[i]->step == step) {
      *res = instruction_buffer[i];
      return SUCCESS;
    }
  }
  return IDX_OVERFLOW;
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

unsigned int frontend_get_max_step() {
  unsigned int i;
  unsigned int max_step = 0;
  for(i = 0; i < idx; i++) {
    if(instruction_buffer[i]->step > max_step && instruction_buffer[i]->step != UINT_MAX) {
      max_step = instruction_buffer[i]->step;
    }
  }
  return max_step;
}

unsigned int frontend_augment_steps_starting_at_addr(unsigned int addr) {
  unsigned int next_available = frontend_get_max_step() + 1;
  unsigned int res = next_available;
  unsigned int i = addr-1;
  for(; i < idx; i++) {
    instruction_buffer[i]->step = next_available++;
  }
  return next_available;
}
