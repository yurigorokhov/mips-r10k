#include "functional.h"

functional_entry* head = NULL;
functional_entry* next = NULL;

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
  queue_in_next(next_alu1, FUNCTIONAL_INTEGER_CYCLES);
  instr* next_alu2 = instr_queue_get_ready_int_instr(1, false);
  queue_in_next(next_alu2, FUNCTIONAL_INTEGER_CYCLES);
  instr* next_fp_add = instr_queue_get_ready_fp_instr(true);
  queue_in_next(next_fp_add, FUNCTIONAL_FP_CYCLES);
  instr* next_fp_mul = instr_queue_get_ready_fp_instr(false);
  queue_in_next(next_fp_mul, FUNCTIONAL_FP_CYCLES);
  instr* next_load_store = instr_queue_get_ready_addr_instr();
  queue_in_next(next_load_store, FUNCTIONAL_LOAD_STORE_CYCLES);
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

  // decrease everyone cycles left, and make sure they are in proper stage
  current = head;
  while(current != NULL) {
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
    if(current->cycles_left == 1) {
      
      // if this is a branch, check if it mispredicted
      if(BRANCH == current->instruction->op && 1 == current->instruction->extra) {
	backend_branch_mispredict(current->instruction);
      }
      active_list_set_instr_ready(current->instruction);
    }
    current = current->next;
  }
  
  // check instructions that are completely done
  current = head;
  while(current != NULL) {
    if(current->cycles_left == 0) {
      remove_head(current->instruction);
    }
    current = current->next;
  }

  // clear next
  next = NULL;
}


