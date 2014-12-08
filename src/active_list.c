#include "active_list.h"

static unsigned int size = 0;
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

char active_list_is_full() {
  if(ACTIVE_LIST_SIZE == size) {
    return 1;
  }
  return 0;
}

char active_list_is_empty() {
  return (size == 0) ? 1 : 0;
}

error_code active_list_add(instr* instruction) {
  if(active_list_is_full()) {
    return ACTIVE_LIST_FULL;
  }
  active_list_entry* entry = malloc(sizeof(active_list_entry));
  entry->instruction = instruction;
  entry->next = NULL;
  active_list_entry* newest = get_newest_entry();
  if(NULL == newest) {
    head = entry;
  } else {
    newest->next = entry;
  }
  size++;
  return SUCCESS;
}
