#include "fetch_stage.h"

static fetch_stage_entry* head = NULL;

unsigned int __calc_fetch_stage() {

  // how many instructions we fetch depends on if
  // there is room in the decode buffer
  return min(decode_buffer_free_spots_next_clock(), BACKEND_DISPATCH_PER_CYCLE);
}

void __edge_fetch_stage_add_instr(instr* instruction) {

  fetch_stage_entry* curr = head;
  while(NULL != curr) {
    if(curr->cycles_left > 0) {
      curr->cycles_left--;
    }
    curr = curr->next;
  }

  fetch_stage_entry* entry = malloc(sizeof(fetch_stage_entry));
  entry->instruction = instruction;
  entry->instruction->stage = FETCH;
  entry->cycles_left = FETCH_CYCLES;
  entry->next = NULL;
  if(NULL == head) {
    head = entry;
  } else {
    fetch_stage_entry* current = head;
    while(NULL != current->next) { current = current->next; }
    current->next = entry;
  }
}

instr* fetch_get_ready_instr(unsigned int skip) {
  if(NULL == head) { return NULL; }
  fetch_stage_entry* current = head;
  unsigned int i = 0;
  while(NULL != current) {
    if(current->cycles_left > 1) {
      return NULL;
    }
    if(skip == i) {
      return current->instruction;
    }
    ++i;
    current = current->next;
  }
  return NULL;
}

void fetch_stage_remove_instr(instr* instruction) {
  fetch_stage_entry* current = head;
  fetch_stage_entry* prev = current;
  while(NULL != current && current->instruction->addr != instruction->addr) {
    prev = current;
    current = current->next;
  }
  if(NULL == current) return;
  if(head == current) {
    head = current->next;
  } else {
    prev->next = current->next;
  }
  free(current);
}

void fetch_stage_handle_mispredict() {
  if(NULL == head) return;
  fetch_stage_entry* current = head;
  fetch_stage_entry* prev;
  head = NULL;
  while(NULL != current) {
    prev = current;
    current = current->next;
    free(prev);
  }
}
