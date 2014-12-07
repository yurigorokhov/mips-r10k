#include "backend.h"

static unsigned int backend_cycle = 0;
static unsigned int pc_addr = 1;

//TODO: return BACKEND_FINISHED
error_code backend_step() {
  backend_cycle++;
  
  // fetch next instructions
  instr* instructions[BACKEND_DISPATCH_PER_CYCLE];
  error_code code;
  unsigned int i = 0;
  while(i < BACKEND_DISPATCH_PER_CYCLE && 
	SUCCESS == (code = frontend_getinsr(pc_addr, &instructions[i]))) {
    i++;
  }

  // dispatch instructions
  if(i > 0) {
    unsigned int j;
    for(j = 0; j <= i; ++j) {
      if(!active_list_is_full() 
	 && !instr_queue_is_full(instructions[j]) 
	 && !reg_map_is_full()) {

	// enqueue instruction
	code = instr_queue_add(instructions[j]);
	if(SUCCESS != code) {
	  return code;
	}

	// update register mapping
	code = map_registers_for_instruction(instructions[j]);
	if(SUCCESS != code) {
	  return code;
	}

	// we dispatched successfully, increment pc counter
	pc_addr++;  
      }
    }
  }

  // functional step
  
  // if the active list is empty, we are done :)
  if(active_list_is_empty()) {
    return BACKEND_FINISHED;
  }
  return SUCCESS;
}
