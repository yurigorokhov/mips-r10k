#include "backend.h"

static unsigned int backend_cycle = 0;
static unsigned int pc_addr = 1;
static char instructions_left = 1;

//TODO: return BACKEND_FINISHED
error_code backend_step() {
  backend_cycle++;
  
  // fetch next instructions
  if(instructions_left) {
    instr* instructions[BACKEND_DISPATCH_PER_CYCLE];
    error_code code;
    unsigned int i = 0;
    while(i < BACKEND_DISPATCH_PER_CYCLE && 
     	  SUCCESS == (code = frontend_getinsr(pc_addr, &instructions[i]))) {
      i++;
    }
    if(IDX_OVERFLOW == code) {
      instructions_left = 0;
    }

    // dispatch instructions
    // TODO: incr PC counter upon successful dispatch
    if(i > 0) {
      
    }
  }
  return SUCCESS;
}
