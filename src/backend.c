#include "backend.h"

static unsigned int backend_cycle = 0;
static unsigned int pc_addr = 1;
static unsigned int backend_step = 1;
static instr* instr_mispredicted = NULL;

// for keeping history of what happens at each cycle
static history_step* history;

unsigned int record_history();

void backend_branch_mispredict(instr* instruction) {
  instr_mispredicted = instruction;
}

error_code backend_cycle_step() {
  unsigned int i;
  instr* instruction;
  error_code code;
  backend_cycle++;
  
  // check if we had a mispredict last cycle
  if(NULL != instr_mispredicted) {

    /* 
     * active_list_handle_mispredict:
     * 
     * Clears out active list
     * Frees up regs
     */
    active_list_handle_mispredict(instr_mispredicted);
    instr_queue_handle_mispredict(instr_mispredicted);
    decode_buffer_handle_mispredict();
    fetch_stage_handle_mispredict();

    // reset stage for all later instructions
    i = instr_mispredicted->addr;
    while(SUCCESS == (code = frontend_getinstr(i++, &instruction))) {
      instruction->stage = NONE;
      i++;
    }

    // set the extra bit to 0 so that we don't livelock
    instr_mispredicted->extra = 0;
    pc_addr = instr_mispredicted->addr;
    backend_step = frontend_augment_steps_starting_at_addr(instr_mispredicted->addr); 
    instr_mispredicted = NULL;
  }

  // calc all stages
  __calc_committer();
  __calc_functional_units();
  __calc_instr_queue();
  __calc_decode_buffer();
  unsigned int instructions_we_can_fetch = __calc_fetch_stage();

  // edge all stages
  __edge_committer();
  __edge_functional_units();
  __edge_instr_queue();
  __edge_decode_buffer();

  // fetch new instructions
  i = 0;
  while(i < instructions_we_can_fetch && 
	SUCCESS == (code = frontend_getinstr(pc_addr++, &instruction))) {
    __edge_fetch_stage_add_instr(instruction);
    instruction->step = backend_step++;
    i++;
  }
  if(0 == record_history()) {
    return BACKEND_FINISHED;
  }
  return SUCCESS;
}

unsigned int record_history() {
  
  // foreach instruction that has a status, record it!
  instr* instruction;
  instr* instruction_copy;
  instr_history_entry* entry = NULL;
  unsigned int i = 0;
  unsigned int outstanding_instructions = 0;
  unsigned int max_step = frontend_get_max_step();
  unsigned int step = 1;
  while(step <= max_step) {
    if(SUCCESS != frontend_getinstr_with_step(step, &instruction)) {
      step++;
      continue;
    }

    // copy instruction for safe-keeping
    instruction_copy = malloc(sizeof(instr));
    memcpy(instruction_copy, instruction, sizeof(instr));

    // calculate if we have any instructions who have yet to commit
    if(instruction->stage != COMMITTED) {
      outstanding_instructions++;
    }

    // create a new history entry
    entry = malloc(sizeof(instr_history_entry));
    entry->next = NULL;
    entry->cycle_num = backend_cycle;
    entry->instruction = instruction_copy;

    // find step entry
    history_step* step_entry;
    history_step* step_prev;
    if(history == NULL) {
      
      // brand new step
      step_entry = malloc(sizeof(history_step));
      step_entry->step = step;
      step_entry->next = NULL;
      step_entry->entry = entry;
      history = step_entry;
    } else {
      
      // find the right insertion point
      step_entry = history;
      while(step_entry != NULL && step_entry->step != step) { 
	step_prev = step_entry;
	step_entry = step_entry->next; 
      }
      if(step_entry == NULL) {
	step_entry = malloc(sizeof(history_step));
	step_entry->step = step;
	step_entry->next = NULL;
	step_entry->entry = entry;
	step_prev->next = step_entry;
      } else {
	
	// we have found the right step, add to the end
	instr_history_entry* curr_hist = step_entry->entry;
	while(curr_hist->next != NULL) { curr_hist = curr_hist->next; }
	curr_hist->next = entry;
      }
    }
    step++;
  }
  return outstanding_instructions;
}

history_step* get_history() {
  return history;
}

unsigned int backend_get_cycle() {
  return backend_cycle;
}
