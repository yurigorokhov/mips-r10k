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
    }
    __instructions_to_commit_size = 0;
  }
}

unsigned int how_many_will_commit_next_clock() {
  return __instructions_to_commit_size;
}
