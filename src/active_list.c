#include "active_list.h"

static active_list_entry* head = NULL;

active_list_entry* get_newest_entry() {
  if(NULL == head) return NULL;
  active_list_entry* newest = head;
  while(newest->next != NULL) {
    newest = newest->next;
  }
  return newest;
}

unsigned int size() {
  unsigned int size = 0;
  active_list_entry* current = head;
  while(NULL != current) {
    current = current->next;
    ++size;
  }
  return size;
}

char active_list_is_full() {
  return (ACTIVE_LIST_SIZE == size()) ? 1 : 0;
}

char active_list_is_empty() {
  return (NULL == head) ? 1 : 0;
}

error_code active_list_add(instr* instruction) {
  if(active_list_is_full()) {
    return ACTIVE_LIST_FULL;
  }
  
  // map the instruction to a physical register
  phys_reg physical = UINT_MAX;
  if(BRANCH != instruction->op && STORE != instruction->op) {
    physical = reg_map_assign(instruction);
  }

  active_list_entry* entry = malloc(sizeof(active_list_entry));
  entry->instruction = instruction;
  entry->next = NULL;
  entry->is_ready_on_next_clock = 0;
  entry->is_res_ready_on_next_clock = false;
  entry->physical = physical;
  active_list_entry* newest = get_newest_entry();
  if(NULL == newest) {
    head = entry;
  } else {
    newest->next = entry;
  }
  return SUCCESS;
}

void active_list_set_instr_res_ready(instr* instruction) {
  if(NULL == head) return;
  active_list_entry* current = head;
  while(NULL != current) {
    if(current->instruction->addr == instruction->addr) {
      current->is_res_ready_on_next_clock = true;
      return;
    }
    current = current->next;
  }
}

void active_list_set_instr_ready(instr* instruction) {
  if(NULL == head) return;
  active_list_entry* current = head;
  while(NULL != current) {
    if(current->instruction->addr == instruction->addr) {
      current->is_ready_on_next_clock = 1;
      return;
    }
    current = current->next;
  }
}

bool active_list_is_instr_ready(instr* instruction) {
  if(NULL == head) return false;
  active_list_entry* current = head;
  while(NULL != current) {
    if(current->instruction->addr == instruction->addr) {
      return current->is_res_ready_on_next_clock;
    }
    current = current->next;
  }
  return false;
}

instr* active_list_get_instr_ready(unsigned int skip) {
  if(NULL == head) { return NULL; }
  active_list_entry* current = head;
  unsigned int i = 0;
  while(NULL != current) {
    if(!current->is_ready_on_next_clock) {
      
      // no further instructions can commit
      break;
    }
    if(skip == i) {
      return current->instruction;
    }
    ++i;
    current = current->next;
  }
  return NULL;
}

void active_list_commit_instruction(instr* instruction) {
  active_list_entry* current = head;
  active_list_entry* prev = current;
  while(NULL != current && current->instruction->addr != instruction->addr) {
    prev = current;
    current = current->next;
  }
  if(NULL == current) return;
  if(current == head) {
    head = current->next;
  } else {
    prev->next = current->next;
  }
  free(current);
}

unsigned int active_list_how_many_spots_next_clock() {
  return min(ACTIVE_LIST_SIZE - size() + how_many_will_commit_next_clock(), ACTIVE_LIST_SIZE);
}

void active_list_handle_mispredict(instr* instruction) {
  
  // remove any newer instructions
  if(NULL == head) return;
  active_list_entry* current = head;
  active_list_entry* prev;
  while(NULL != current 
	&& instruction->addr != current->instruction->addr) {
    prev = current;
    current = current->next;
  }
  if(NULL == current) return;
  if(current == head) {
    head = NULL;
  } else {
    prev->next = NULL;
  }
  
  // clean up all the others
  do {
    prev = current;
    current = current->next;
    if(prev->instruction->rd != UINT_MAX)
      reg_map_free_by_logical(prev->instruction->rd);
    free(prev);
  } while(NULL != current->next);
}
