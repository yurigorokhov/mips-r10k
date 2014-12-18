#include "committer.h"

static instr* __instructions_to_commit[BACKEND_DISPATCH_PER_CYCLE];
static unsigned int __instructions_to_commit_size = 0;
static instr* load_store_mem_cycle = NULL;

void __calc_committer() {
  
  // check active list for instructions that are ready to commit
  instr* instruction;
  bool has_done_mem_cycle = true;
  load_store_mem_cycle = NULL;
  while(__instructions_to_commit_size < BACKEND_DISPATCH_PER_CYCLE) {
    instruction = active_list_get_instr_ready(__instructions_to_commit_size, &has_done_mem_cycle);
    if(NULL == instruction) {
      break;
    }
    if((instruction->op == LOAD || instruction->op == STORE) && !has_done_mem_cycle) {
	load_store_mem_cycle = instruction;
	break;
    }
    __instructions_to_commit[__instructions_to_commit_size++] = instruction;
  }
}

void __edge_committer() {
  if(load_store_mem_cycle != NULL) {
    load_store_mem_cycle->stage = MEM;
    active_list_set_mem_cycle(load_store_mem_cycle);
    load_store_mem_cycle = NULL;
  }
  if(__instructions_to_commit_size > 0) {
    unsigned int i = 0;
    while(i < __instructions_to_commit_size) {
      __instructions_to_commit[i]->stage = COMMITTED;

      // if this is a load or store, we can remove from queue
      if(__instructions_to_commit[i]->op == STORE 
	 || __instructions_to_commit[i]->op == LOAD) {
	instr_queue_remove(__instructions_to_commit[i]);
      }
      
      // commit in active list
      active_list_commit_instruction(__instructions_to_commit[i]);

      // return register to free list
      logi_reg reg = get_dest_reg(__instructions_to_commit[i]);
      if(reg != UINT_MAX) {
	reg_map_free_by_logical(reg);
      }
      i++;
    }
    __instructions_to_commit_size = 0;
  }
}

unsigned int how_many_will_commit_next_clock() {
  return __instructions_to_commit_size;
}

unsigned int how_many_addr_will_commit_next_clock() {
  unsigned int i = 0;
  unsigned int count = 0;
  while(i < __instructions_to_commit_size) {
    if(__instructions_to_commit[i]->op == LOAD
       || __instructions_to_commit[i]->op == STORE) {
      count++;
    }
    i++;
  }
  return count;
}
