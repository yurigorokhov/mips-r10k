#include <stdlib.h>
#include "frontend.h"
#include "instr.h"
#include "error.h"

static instr** instruction_buffer;
static unsigned int idx = 0;
static unsigned int capacity = 0;

void frontend_init(unsigned int size) {
  instruction_buffer = (instr**)malloc(size*sizeof(instr*));
  capacity = size;
}

result frontend_enqueue(instr* instruction) {
  if(idx == capacity - 1) {
    instruction_buffer = (instr**)realloc(instruction_buffer, 
					  100+(capacity*sizeof(instr*)));
    capacity += 100;
  }
  instruction_buffer[idx++] = instruction;
  return get_success();
}

void frontend_clean() {
  free(instruction_buffer);
}
