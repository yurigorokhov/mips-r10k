#include "functional.h"

// instructions to be picked next
static instr* _next_instr_alu1 = NULL;
static instr* _next_instr_alu2 = NULL;

// instructions currently executing
static functional_entry* _curr_instr_alu1 = NULL;
static functional_entry* _curr_instr_alu2 = NULL;

// used during calc to record how many instructions will be added next cycle
static unsigned int free_int_spots = 0;

void __calc_functional_units() {
  
  // pick items that will be executed from instruction queues if we have room
  if(NULL == _curr_instr_alu1 || 1 == _curr_instr_alu1->cycles_left) {
    _next_instr_alu1 = instr_queue_get_ready_int_instr(0, true); free_int_spots++;
  }
  if(NULL == _curr_instr_alu2 || 1 == _curr_instr_alu2->cycles_left) {
    _next_instr_alu2 = instr_queue_get_ready_int_instr(1, false); free_int_spots++;
  }
}

void __edge_functional_units() { 

  // remove items from instruction queues and execute them!
  if((NULL == _curr_instr_alu1 || _curr_instr_alu1->cycles_left == 1) 
     && NULL != _next_instr_alu1) {
    instr_queue_remove(_next_instr_alu1);
    _curr_instr_alu1 = malloc(sizeof(functional_entry));
    _curr_instr_alu1->instruction = _next_instr_alu1;
    _curr_instr_alu1->instruction->stage = EXECUTE;
    _curr_instr_alu1->cycles_left = FUNCTIONAL_INTEGER_CYCLES;
  } else if(NULL != _curr_instr_alu1 && _curr_instr_alu1->cycles_left > 1) {
    _curr_instr_alu1->cycles_left--;
  }
  if((NULL == _curr_instr_alu2 || _curr_instr_alu2->cycles_left == 1) 
     && NULL != _next_instr_alu2) {
    instr_queue_remove(_next_instr_alu2);
    _curr_instr_alu2 = malloc(sizeof(functional_entry));
    _curr_instr_alu2->instruction = _next_instr_alu2;
    _curr_instr_alu2->instruction->stage = EXECUTE;
    _curr_instr_alu2->cycles_left = FUNCTIONAL_INTEGER_CYCLES;
  } else if(NULL != _curr_instr_alu2 && _curr_instr_alu2->cycles_left > 1) {
    _curr_instr_alu2->cycles_left--;
  }

  // set instructions that will be ready next clock cycle
  if(NULL != _curr_instr_alu1 && 1 == _curr_instr_alu1->cycles_left) {
    active_list_set_instr_ready(_curr_instr_alu1->instruction);

    // check if a branch mispredicted
    if(BRANCH == _curr_instr_alu1->instruction->op &&  1 == _curr_instr_alu1->instruction->extra) {
      
      // this branch mispredicted, the next cycle will be a mispredict bubble
      backend_branch_mispredict(_curr_instr_alu1->instruction);
    }
  }
  if(NULL != _curr_instr_alu2 && 1 == _curr_instr_alu2->cycles_left) {
    active_list_set_instr_ready(_curr_instr_alu2->instruction);
  }

  // reset next
  _next_instr_alu1 = NULL;
  _next_instr_alu2 = NULL;
  free_int_spots = 0;
}

unsigned int functional_free_int_spots_next_clock() {
  return free_int_spots;
}

