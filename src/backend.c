#include "backend.h"

static unsigned int backend_cycle = 0;
static unsigned int pc_addr = 1;
static instr* instr_mispredicted = NULL;

// for keeping history of what happens at each cycle
static instr_history_entry** history;

unsigned int record_history();

void backend_init() {
  unsigned int total_instructions = frontend_get_instr_count();
  history = malloc(total_instructions * sizeof(instr_history_entry*));
  unsigned int i = 0;
  while(i < total_instructions) {
    history[i] = NULL;
    ++i;
  }
  reg_map_init();
}

void backend_branch_mispredict(instr* instruction) {
  instr_mispredicted = instruction;
}

error_code backend_cycle_step() {
  backend_cycle++;

  // check if we had a mispredict last cycle
  if(NULL != instr_mispredicted) {
    
    //TODO: this needs to be finished!
    active_list_handle_mispredict(instr_mispredicted);
    instr_queue_handle_mispredict(instr_mispredicted);
    // clean reg_map
    // clean decode buffer
    // clean functional units

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
  unsigned int i;
  error_code code;
  instr* instruction;
  while(i < instructions_we_can_fetch && 
	SUCCESS == (code = frontend_getinstr(pc_addr++, &instruction))) {
    __edge_fetch_stage_add_instr(instruction);
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
  instr_history_entry* entry;
  unsigned int i = 0;
  unsigned int addr = 1;
  unsigned int outstanding_instructions = 0;
  while(SUCCESS == frontend_getinstr(addr, &instruction)) {
    
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

    // add entry to history
    if(NULL == history[addr-1]) {
      history[addr-1] = entry;
    } else {
      
      // find the last entry and append there
      instr_history_entry* current = history[addr-1];
      while(NULL != current->next) { current = current->next; }
      
      current->next = entry;
    }
    addr++;
  }
  return outstanding_instructions;
}

instr_history_entry** get_history() {
  return history;
}

unsigned int backend_get_cycle() {
  return backend_cycle;
}
