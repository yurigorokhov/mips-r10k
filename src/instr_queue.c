#include "instr_queue.h"

// integer queue
static instr_queue_entry* int_queue_head = NULL;

// address queue
static instr_queue_entry* addr_queue_head = NULL;

// floating point queue
static instr_queue_entry* fp_queue_head = NULL;

static instr* _queue_lookahead[BACKEND_DISPATCH_PER_CYCLE] = { NULL };
static size_t _queue_lookahead_size = 0;

//--- Functions ---
size_t get_queue_size(instr_queue_entry *q) {
  if(NULL == q) { return 0; }
  size_t size = 0;
  while(NULL != q) {
    q = q->next;
    ++size;
  }
  return size;
}

instr_queue_entry** get_queue_by_type(instr* instruction, size_t* size) {
  switch(instruction->op) {
  case INTEGER:
    *size = INT_QUEUE_SIZE;
    return &int_queue_head;
  case LOAD:
  case STORE:
    *size = ADDR_QUEUE_SIZE;
    return &addr_queue_head;
  case FPADD:
  case FPMUL:
    *size = FP_QUEUE_SIZE;
    return &fp_queue_head;
  case BRANCH:

    //TODO!!
    break;
  } 
}

void instr_queue_remove(instr *instruction) {
  size_t s;
  instr_queue_entry** head_ptr = get_queue_by_type(instruction, &s);
  instr_queue_entry* current = *head_ptr;
  instr_queue_entry* prev = current;
  while(NULL != current && current->instruction->addr != instruction->addr) {
    prev = current;
    current = current->next;
  }
  if(NULL == current) {
    return;
  }
  if(current == *head_ptr) {
    *head_ptr = current->next;
  } else {
    prev->next = current->next;
  }
  free(current);
}

instr* instr_queue_get_ready_int_instr(unsigned int skip) {
  if(NULL == int_queue_head) { return NULL; }
  instr_queue_entry* current = int_queue_head;
  unsigned int i = 0;
  while(NULL != current->next) { 
    current = current->next; 
    if(1) { //TODO: this should be a ready check
      if(i == skip) {
	return current->instruction;
      }
      ++i;
    }
  }
}

void __calc_instr_queue() {
  
  // get possible instructions that will fill the queue from decode buffer
  instr* next_instr;
  unsigned int i = 0;
  while(i < BACKEND_DISPATCH_PER_CYCLE && NULL != (next_instr = decode_buffer_get_next_ready_instr(i))) {
    _queue_lookahead[_queue_lookahead_size++] = next_instr;
    ++i;
  }
}

void insert_instruction(instr* instruction, instr_queue_entry** queue) {
  instr_queue_entry* entry = malloc(sizeof(instr_queue_entry));
  entry->instruction = instruction;
  entry->next = NULL;
  if(NULL == *queue) {
    *queue = entry;
  } else {
    instr_queue_entry* current = *queue;
    while(NULL != current->next) { current = current->next; }
    current->next = entry;
  }
}

void __edge_instr_queue() {
  if(_queue_lookahead_size > 0) {
    unsigned int i;
    for(i = 0; i < _queue_lookahead_size; ++i) {
      size_t queue_size;
      instr_queue_entry** queue = get_queue_by_type(_queue_lookahead[i], &queue_size);
      if(queue_size == get_queue_size(*queue)) {

	// this queue is full, skip instruction (it stays in decode buffer)
	continue;
      }
      
      // insert into queue & delete from decode buffer
      insert_instruction(_queue_lookahead[i], queue);
      decode_buffer_remove_instruction(_queue_lookahead[i]);
    }
    _queue_lookahead_size = 0;
  }
}
