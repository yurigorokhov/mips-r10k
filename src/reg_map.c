#include "reg_map.h"

void add_to_free_list(phys_reg);
void add_to_busy_list(phys_reg physical, logi_reg logical, instr*);

free_list_entry* free_list = NULL;
busy_list_entry* busy_list = NULL;

void reg_map_init() {
  
  // populate free list
  unsigned int i;
  for(i = 0; i < PHYS_REGS; ++i) {
    add_to_free_list((phys_reg)i);
  }
}

phys_reg reg_map_assign(instr* instruction) {
  phys_reg physical = free_list->physical;
  //TODO: printf("\nassigning phys %i", physical);

  // remove from free list
  free_list_entry* old_head = free_list;
  free_list = free_list->next;
  free(old_head);

  // add to busy list
  add_to_busy_list(physical, instruction->rd, instruction);
  return physical;
}

void add_to_free_list(phys_reg reg_num) {
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
