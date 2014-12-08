#include "functional.h"

// instructions to be picked next
static instr* _next_instr_alu1 = NULL;
static instr* _next_instr_alu2 = NULL;

// instructions currently executing
static functional_entry* _curr_instr_alu1 = NULL;
static functional_entry* _curr_instr_alu2 = NULL;

void __calc_functional_units() {
  
  // pick items that will be executed from instruction queues
  _next_instr_alu1 = instr_queue_get_ready_int_instr(0);
  _next_instr_alu2 = instr_queue_get_ready_int_instr(1);
}

void __edge_functional_units() { 

  // remove items from instruction queues and execute them!
  if(NULL == _curr_instr_alu1 && NULL != _next_instr_alu1) {
    instr_queue_remove(_next_instr_alu1);
    _curr_instr_alu1 = malloc(sizeof(functional_entry));
    _curr_instr_alu1->instruction = _next_instr_alu1;
  }
  if(NULL == _curr_instr_alu2 && NULL != _next_instr_alu2) {
    instr_queue_remove(_next_instr_alu2);
    _curr_instr_alu2 = malloc(sizeof(functional_entry));
    _curr_instr_alu2->instruction = _next_instr_alu2;
  }

  //if(_curr_instr_alu1 != NULL)
  //printf("\nALU1: instr %i", _curr_instr_alu1->instruction->addr);
  //if(_curr_instr_alu2 != NULL)
  //printf("\nALU1: instr %i", _curr_instr_alu2->instruction->addr);

  // reset next
  _next_instr_alu1 = NULL;
  _next_instr_alu2 = NULL;
}

