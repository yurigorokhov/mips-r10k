#include "branch_stack.h"

static bs_entry* head = NULL;

void bs_add(instr* instruction) {
  if(instruction->op != BRANCH) return;
  bs_entry* entry = malloc(sizeof(bs_entry));
  entry->instruction = instruction;
  entry->next = NULL;
  entry->is_ready_on_next_clock = false;
  if(NULL == head) {
    head = entry;
  } else {
    bs_entry* curr = head;
    bs_entry* prev;
    while(curr != NULL) {
      if(curr->instruction->addr == instruction->addr) {
	free(entry);
	return;
      }
      prev = curr;
      curr = curr->next;
    }
    prev->next = entry;
  }
}

void bs_remove(instr* instruction) {
  if(instruction->op != BRANCH || NULL == head) return;
  bs_entry* curr = head;
  bs_entry* prev;
  while(curr != NULL && curr->instruction->addr != instruction->addr) {
    prev = curr;
    curr = curr->next;
  }
  if(NULL == curr) return;
  if(curr == head) {
    head = head->next;
  } else {
    prev->next = curr->next;
  }
  free(curr);
}

void bs_set_instr_ready(instr* instruction) {
  if(instruction->op != BRANCH || NULL == head) return;
  bs_entry* curr = head;
  while(curr != NULL && curr->instruction->addr != instruction->addr) {
    curr = curr->next;
  }
  if(NULL == curr) return;
  curr->is_ready_on_next_clock = true;
}

unsigned int bs_size_next_clock() {
  unsigned int i = 0;
  bs_entry* curr = head;
  if(NULL == head) {
    return 0;
  }
  while(curr != NULL) {
    if(!curr->is_ready_on_next_clock) {
      i++;
    }
    curr = curr->next;
  }
  return i;
}
