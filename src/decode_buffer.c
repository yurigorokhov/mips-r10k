#include "decode_buffer.h"

static decode_buffer_entry* decode_buffer_head = NULL; 

static instr* __calc_scheduled_to_be_added[DECODE_BUFFER_SIZE] = { NULL };
static unsigned int __calc_scheduled_size = 0;

unsigned int get_size() {
  decode_buffer_entry* current = decode_buffer_head;
  unsigned int i = 0;
  while(NULL != current) {
    current = current->next;
    ++i;
  }
  return i;
}

bool is_full() {
  return (DECODE_BUFFER_SIZE == get_size()) ? true : false;
}

void add_instruction(instr* instruction) {
  decode_buffer_entry* entry = malloc(sizeof(decode_buffer_entry));
  instruction->stage = DECODE;
  entry->instruction = instruction;
  entry->next = NULL;
  entry->cycles_left = DECODE_CYCLES;
  if(NULL == decode_buffer_head) {
    decode_buffer_head = entry;
    return;
  }
  decode_buffer_entry* current = decode_buffer_head;
  while(NULL != current->next) { current = current->next; }
  current->next = entry;
}

void __calc_decode_buffer() {
  instr* instruction;
  while(__calc_scheduled_size < DECODE_BUFFER_SIZE 
	&& NULL != (instruction = (instr*)fetch_get_ready_instr(__calc_scheduled_size))) {
    __calc_scheduled_to_be_added[__calc_scheduled_size] = instruction;
    __calc_scheduled_size++;
  }
}

void __edge_decode_buffer() {
  unsigned int i = 0;
  if(__calc_scheduled_size > 0) {
    while(i < __calc_scheduled_size && !is_full()) {
      add_instruction(__calc_scheduled_to_be_added[i]);
      
      // instruction is being added to decode buffer >> DECODE
      __calc_scheduled_to_be_added[i]->stage = DECODE;
      
      // remove from fetch buffer
      fetch_stage_remove_instr(__calc_scheduled_to_be_added[i]);
      ++i;
    }
    __calc_scheduled_size = 0;
  }
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
  decode_buffer_entry* prev = NULL;
  while(NULL != current && current->instruction->addr != instruction->addr) {
    prev = current;
    current = current->next;
  }
  if(NULL == current) return;
  if(current == decode_buffer_head) {
    decode_buffer_head = current->next;
  } else {
    prev->next = current->next;
  }
  free(current);
}

unsigned int decode_buffer_free_spots_next_clock() {
  unsigned int free_active_list = active_list_how_many_spots_next_clock();
  unsigned int free_int_spots = instr_queue_free_int_spots_next_clock();
  unsigned int free_addr_spots = instr_queue_free_addr_spots_next_clock();
  unsigned int free_fp_spots = instr_queue_free_fp_spots_next_clock();
  unsigned int count = 0;
  decode_buffer_entry* current = decode_buffer_head;
  while(NULL != current && free_active_list > 0) {
    switch(current->instruction->op) {
    case INTEGER:
    case BRANCH: //TODO: also check branch stack
      if(0 == free_int_spots) goto exitloop;
      free_int_spots--;
      break;
    case LOAD:
    case STORE:
      if(0 == free_addr_spots) goto exitloop;
      free_addr_spots--;
      break;
    case FPADD:
    case FPMUL:
      if(0 == free_fp_spots) goto exitloop;
      free_fp_spots--;
      break;
    }
    current = current->next;
    free_active_list--;
    count++;
  }
exitloop:
  return min(DECODE_BUFFER_SIZE,  DECODE_BUFFER_SIZE - get_size() + count);
}
