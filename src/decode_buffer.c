#include "decode_buffer.h"

static decode_buffer_entry* decode_buffer_head = NULL; 

static instr* __calc_scheduled_to_be_added[DECODE_BUFFER_SIZE] = { NULL };
static unsigned int __calc_scheduled_size = 0;

unsigned int size() {
  decode_buffer_entry* current = decode_buffer_head;
  unsigned int i = 0;
  while(current != NULL) {
    current = current->next;
    ++i;
  }
  return i;
}

char is_full() {
  return (DECODE_BUFFER_SIZE == size()) ? 1 : 0;
}

void add_instruction(instr* instruction) {
  decode_buffer_entry* entry = malloc(sizeof(decode_buffer_entry));
  entry->instruction = instruction;
  entry->next = NULL;
  entry->cycles_left = FETCH_CYCLES;
  if(NULL == decode_buffer_head) {
    decode_buffer_head = entry;
    return;
  }
  decode_buffer_entry* current = decode_buffer_head;
  while(NULL != current->next) { current = current->next; }
  current->next = entry;
}

error_code __calc_decode_buffer_add(instr* instruction) {
  if(DECODE_BUFFER_SIZE - 1 == __calc_scheduled_size) {
    return DECODE_BUFFER_FULL;
  }
  __calc_scheduled_to_be_added[__calc_scheduled_size++] = instruction;
  return SUCCESS;
}

int __edge_decode_buffer_accept_instructions() {
  int i = 0;
  if(__calc_scheduled_size > 0) {
    while(i < __calc_scheduled_size && !is_full()) {
      add_instruction(__calc_scheduled_to_be_added[i]);
      
      // instruction is being added to decode buffer >> FETCH
      __calc_scheduled_to_be_added[i]->stage = FETCH;
      ++i;
    }
    __calc_scheduled_size = 0;
  }
  return i;
}

instr* decode_buffer_get_next_ready_instr(unsigned int skip) {
  decode_buffer_entry* current = decode_buffer_head;
  if(NULL == current) { return NULL; }
  unsigned int i = 0;
  while(NULL != current) {
    
    // instructions that have 1 cycle left will be ready to move on
    if(1 == current->cycles_left) {
      if(i == skip) {
	return current->instruction;
      }
      ++i;
    }
    current = current->next;
  }
}

void decode_buffer_remove_instruction(instr* instruction) {
  decode_buffer_entry* current = decode_buffer_head;
  decode_buffer_entry* prev = current;
  while(NULL != current && current->instruction->addr != instruction->addr) {
    prev = current;
    current = current->next;
  }
  if(NULL == current) {
    return;
  }
  if(current == decode_buffer_head) {
    decode_buffer_head = current->next;
  } else {
    prev->next = current->next;
  }
  free(current);
}

unsigned int decode_buffer_free_spots_next_clock() {
  unsigned int capacity_downstream = min(active_list_how_many_spots_next_clock(), instr_queue_free_int_spots_next_clock());
  unsigned int capacity = DECODE_BUFFER_SIZE - size() + capacity_downstream;
  return min(DECODE_BUFFER_SIZE, capacity);
}
