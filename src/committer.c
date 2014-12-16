#include "committer.h"

static instr* __instructions_to_commit[BACKEND_DISPATCH_PER_CYCLE];
static unsigned int __instructions_to_commit_size = 0;

void __calc_committer() {
  
  // check active list for instructions that are ready to commit
  instr* instruction;
  while(__instructions_to_commit_size < BACKEND_DISPATCH_PER_CYCLE) {
    instruction = active_list_get_instr_ready(__instructions_to_commit_size);
    if(NULL == instruction) {
      break;
    }
    __instructions_to_commit[__instructions_to_commit_size++] = instruction;
  }
}

void __edge_committer() {
  if(__instructions_to_commit_size > 0) {
    unsigned int i = 0;
    while(i < __instructions_to_commit_size) {
      __instructions_to_commit[i]->stage = COMMITTED;
      active_list_commit_instruction(__instructions_to_commit[i]);

      // if this is a load or store, we can remove from queue
      if(__instructions_to_commit[i]->op == STORE 
	 || __instructions_to_commit[i]->op == LOAD) {
	instr_queue_remove(__instructions_to_commit[i]);
      }

      // return register to free list
      if(__instructions_to_commit[i]->rd != UINT_MAX) {
	reg_map_free_by_logical(__instructions_to_commit[i]->rd);
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
