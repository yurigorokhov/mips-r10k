#include "functional.h"

functional_entry* head = NULL;
functional_entry* next = NULL;
static unsigned int taking_int = 0;
static unsigned int taking_fp = 0;
static unsigned int taking_addr = 0;

void queue_in_next(instr* instruction, unsigned int cycles) {
  if(NULL == instruction) return;
  functional_entry* current = next;
  functional_entry* entry = malloc(sizeof(functional_entry));
  entry->next = NULL;
  entry->instruction = instruction;
  entry->cycles_left = cycles;
  if(NULL == next) {
    next = entry;
  } else {
    while(NULL != current->next) {
      current = current->next;
    }
    current->next = entry;
  }
}

void __calc_functional_units() {
  instr* next_alu1 = instr_queue_get_ready_int_instr(0, true);
  if(NULL != next_alu1) taking_int++;
  queue_in_next(next_alu1, FUNCTIONAL_INTEGER_CYCLES-1);
  instr* next_alu2 = instr_queue_get_ready_int_instr(1, false);
  if(NULL != next_alu2) taking_int++;
  queue_in_next(next_alu2, FUNCTIONAL_INTEGER_CYCLES-1);
  instr* next_fp_add = instr_queue_get_ready_fp_instr(true);
  if(NULL != next_fp_add) taking_fp++;
  queue_in_next(next_fp_add, FUNCTIONAL_FP_CYCLES-1);
  instr* next_fp_mul = instr_queue_get_ready_fp_instr(false);
  if(NULL != next_fp_mul) taking_fp++;
  queue_in_next(next_fp_mul, FUNCTIONAL_FP_CYCLES-1);
  instr* next_load_store = instr_queue_get_ready_addr_instr();
  if(NULL != next_load_store) taking_addr++;
  queue_in_next(next_load_store, FUNCTIONAL_LOAD_STORE_CYCLES-1);
}

void remove_head(instr* instruction) {
  functional_entry* current = head;
  functional_entry* prev;
  while(current != NULL && current->instruction->addr != instruction->addr) {
    prev = current;
    current = current->next;
  }
  if(current == NULL) return;
  if(current == head) {
    head = current->next;
  } else {
    prev->next = current->next;
  }
  free(current);
}

void queue(functional_entry* entry) {
  functional_entry* current = head;
  entry->next = NULL;
  if(current == NULL) {
    head = entry;
  } else {
    while(current->next != NULL) { current = current->next; }
    current->next = entry;
  }
}

void __edge_functional_units() { 
  functional_entry* current;
  functional_entry* prev;

  // check instructions that are completely done
  current = head;
  while(current != NULL) {
    if(current->cycles_left == 0) {
      remove_head(current->instruction);
    }
    current = current->next;
  }

  // decrease everyone cycles left, and make sure they are in proper stage
  current = head;
  while(current != NULL) {

    // FP needs to stick around for 1 more cycle
    if((current->instruction->op == FPADD || current->instruction->op == FPMUL)) {
      if(current->cycles_left == 2) {
	active_list_set_instr_res_ready(current->instruction);
      } else if(current->cycles_left == 1) {
	 current->instruction->stage = FINISHED;
      }
    }
    if(current->cycles_left == 1 && (current->instruction->op == STORE || current->instruction->op == LOAD)) {
      current->instruction->stage = MEM;
    }
    current->cycles_left--;
    prev = current;
    current = current->next;
  }

  // add next instructions
  current = next;
  while(NULL != current) {
    prev = current->next;
    queue(current);
    current->instruction->stage = EXECUTE;

    // if this is not a load or store, remove from queue
    if(current->instruction->op != LOAD && current->instruction->op != STORE) {
      instr_queue_remove(current->instruction);
    }
    current = prev;
  }

  // check those that will be ready on the next clock
  current = head;
  while(current != NULL) {

    // set instructions that will be ready on the next clock
    if(current->cycles_left == 0) {
      
      // if this is a branch, check if it mispredicted
      if(BRANCH == current->instruction->op && 1 == current->instruction->extra) {
	backend_branch_mispredict(current->instruction);
      }
      active_list_set_instr_ready(current->instruction);
      active_list_set_instr_res_ready(current->instruction);
    }
    current = current->next;
  }

  // clear next
  next = NULL;
  taking_addr = 0;
  taking_int = 0;
  taking_fp = 0;
}

unsigned int f_taking_int() {
  return taking_int;
}


unsigned int f_taking_fp() {
  return taking_fp;
}


unsigned int f_taking_addr() {
  return taking_addr;
}

