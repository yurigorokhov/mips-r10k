#include "backend.h"

static unsigned int backend_cycle = 0;
static unsigned int pc_addr = 1;

error_code backend_cycle_step() {
  backend_cycle++;
  
  // calc all stages
  __calc_functional_units();
  __calc_instr_queue();
  
  // calc for decode buffer
  instr* instruction;
  error_code code;
  unsigned int i = 0;
  while(i < BACKEND_DISPATCH_PER_CYCLE && 
	SUCCESS == (code = frontend_getinsr(pc_addr+i, &instruction))) {
    __calc_decode_buffer_add(instruction);
    i++;
  }

  // edge all stages
  __edge_functional_units();
  __edge_instr_queue();

  // decode edge returns how many instructions it was able to accept
  unsigned int num_fetched = __edge_decode_buffer_accept_instructions();
  pc_addr += num_fetched;
  return SUCCESS;
}
