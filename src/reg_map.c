#include "reg_map.h"

void add_to_free_list(phys_reg);
void add_to_busy_list(phys_reg physical, logi_reg logical, instr*);

free_list_entry* free_list = NULL;
busy_list_entry* busy_list = NULL;

/* Note phy0 is always assigned to log0 
 * and is never busy
 */

void reg_map_init() {
  
  // populate free list
  unsigned int i;
  for(i = 1; i < PHYS_REGS; ++i) {
    add_to_free_list((phys_reg)i);
  }
}

logi_reg get_dest_reg(instr* instruction) {
  if(instruction->op == LOAD) 
    return instruction->rt;
  if(instruction->op == STORE || instruction->op == BRANCH)
    return 0;
  return instruction->rd;
}

phys_reg reg_map_assign(instr* instruction) {
  logi_reg reg = get_dest_reg(instruction);
  if(reg == 0) return 0;
  phys_reg physical = free_list->physical;

  // remove from free list
  free_list_entry* old_head = free_list;
  free_list = free_list->next;
  free(old_head);

  // add to busy list
  add_to_busy_list(physical, reg, instruction);
  return physical;
}

void add_to_free_list(phys_reg reg_num) {
  if(reg_num == 0) return;
  free_list_entry* entry = malloc(sizeof(free_list_entry));
  entry->next = NULL;
  entry->physical = reg_num;
  if(NULL == free_list) {
    free_list = entry;
  } else {
    free_list_entry* current = free_list;
    while(NULL != current->next) { current = current->next; }
    current->next = entry;
  }
}

void add_to_busy_list(phys_reg physical, logi_reg logical, instr* instruction) {
  if(logical == 0) {
    return;
  }
  busy_list_entry* entry = malloc(sizeof(busy_list_entry));
  entry->next = NULL;
  entry->physical = physical;
  entry->logical = logical;
  entry->instruction_writing = instruction;
  if(NULL == busy_list) {
    busy_list = entry;
  } else {
    busy_list_entry* current = busy_list;
    while(NULL != current->next) { current = current->next; }
    current->next = entry;
  }
}

instr* reg_map_get_latest_instr_writing_to(logi_reg logical, unsigned int before_addr) {
  busy_list_entry* current = busy_list;
  busy_list_entry* prev = NULL;
  while(NULL != current) {
    if(current->logical == logical && current->instruction_writing->addr < before_addr) {
      prev = current;
    }
    current = current->next;
  }
  if(NULL == prev) return NULL;
  return prev->instruction_writing;
}

void reg_map_free_by_logical(logi_reg logical) {
  if(logical == 0) return;
  busy_list_entry* current = busy_list;
  busy_list_entry* prev = NULL;
  while(NULL != current && current->logical != logical) {
    prev = current;
    current = current->next;
  }
  if(NULL == current) return;

  // add to free list
  add_to_free_list(current->logical);
  if(current = busy_list) {
    busy_list = current->next;
  } else {
    prev->next = current->next;
  }
  free(current);
}
