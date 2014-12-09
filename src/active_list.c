#include "active_list.h"

static active_list_entry* head = NULL;

active_list_entry* get_newest_entry() {
  if(NULL == head) {
    return NULL;
  }
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
    ++i;
  }
  return i;
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
  active_list_entry* entry = malloc(sizeof(active_list_entry));
  entry->instruction = instruction;
  entry->next = NULL;
  entry->is_ready_on_next_clock = 0;
  active_list_entry* newest = get_newest_entry();
  if(NULL == newest) {
    head = entry;
  } else {
    newest->next = entry;
  }
  return SUCCESS;
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
  if(NULL == head) { return; }
  active_list_entry* current = head;
  active_list_entry* prev = current;
  while(NULL != current && current->instruction->addr != instruction->addr) {
    prev = current;
    current = current->next;
  }
  if(current == head) {
    head = current->next;
  } else {
    prev = current->next;
  }
  free(current);
}

unsigned int active_list_how_many_spots_next_clock() {
  return min(ACTIVE_LIST_SIZE - size() + how_many_will_commit_next_clock(), ACTIVE_LIST_SIZE);
}
