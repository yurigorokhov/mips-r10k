#include "instr_queue.h"

bool will_instr_inputs_be_ready(instr* instruction);

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
  case BRANCH:
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

bool will_instr_inputs_be_ready(instr* instruction) {
  instr* writing_instr;
  if(instruction->rs != UINT_MAX) {
    writing_instr = reg_map_get_latest_instr_writing_to(instruction->rs, instruction->addr);
    if(NULL != writing_instr && !active_list_is_instr_ready(writing_instr)) {
      return false;
    }
  }
  if(instruction->rt != UINT_MAX) {
    writing_instr = reg_map_get_latest_instr_writing_to(instruction->rt, instruction->addr);
    if(NULL != writing_instr && !active_list_is_instr_ready(writing_instr)) {
      return false;
    }
  }
  return true;
}

instr* instr_queue_get_ready_addr_instr() {
  if(NULL == addr_queue_head) return NULL;
  instr_queue_entry* current = addr_queue_head;

  // skip those that are not in issue stage
  while(current != NULL && current->instruction->stage >= EXECUTE) { 
    current = current->next;
  }
  if(NULL == current) return NULL;
  // TODO: can we execute out of order?
  // always process the next load/store in order
  if(current->cycles_left > 1 
     || !will_instr_inputs_be_ready(current->instruction)) return NULL;
  return current->instruction;
}

instr* instr_queue_get_ready_int_instr(unsigned int skip, bool is_alu_1) {
  if(NULL == int_queue_head) return NULL;
  instr_queue_entry* current = int_queue_head;
  unsigned int i = 0;
  while(NULL != current) { 
    if(1 == current->cycles_left
       && will_instr_inputs_be_ready(current->instruction)) {
      if(i == skip) {
	
	// only ALU1 can resolve branches
	if(is_alu_1 || BRANCH != current->instruction->op) {
	  return current->instruction;
	}
      } else {
	++i;
      }
      
    }
    current = current->next;
  }
  return NULL;
}

void __calc_instr_queue() {
  
  // get possible instructions that will fill the queue from decode buffer
  instr* next_instr;
  unsigned int i = 0;
  unsigned int free_spots_next_clock = instr_queue_free_int_spots_next_clock();
  while(i < free_spots_next_clock && NULL != (next_instr = decode_buffer_get_next_ready_instr(i))) {
    _queue_lookahead[_queue_lookahead_size++] = next_instr;
    ++i;
  }
}

void insert_instruction(instr* instruction, instr_queue_entry** queue) {
  instr_queue_entry* entry = malloc(sizeof(instr_queue_entry));
  entry->instruction = instruction;
  entry->cycles_left = ISSUE_CYCLES;
  
  // STAGE >> ISSUE
  entry->instruction->stage = ISSUE;
  entry->next = NULL;
  if(NULL == *queue) {
    *queue = entry;
  } else {

    // if this is a branch, it gets priority so put it at the front
    if(BRANCH == instruction->op) {
      entry->next = *queue;
      *queue = entry;
    } else {

      // schedule at the end
      instr_queue_entry* current = *queue;
      while(NULL != current->next) { current = current->next; }
      current->next = entry;
    }
  }
}

void __edge_instr_queue() {
  if(_queue_lookahead_size > 0) {
    unsigned int i;
    for(i = 0; i < _queue_lookahead_size; ++i) {
      
      // if the active instruction list is full, there is nothing we can do
      if(active_list_is_full()) {
	break;
      }

      // check the proper queue to make sure there is room
      size_t queue_size;
      instr_queue_entry** queue = get_queue_by_type(_queue_lookahead[i], &queue_size);
      if(queue_size == get_queue_size(*queue)) {

	// this queue is full, skip instruction (it stays in decode buffer)
	continue;
      }
      
      // insert into queue & delete from decode buffer
      insert_instruction(_queue_lookahead[i], queue);
      decode_buffer_remove_instruction(_queue_lookahead[i]);

      // STAGE >> DECODE
      _queue_lookahead[i]->stage = DECODE;

      // add to active list
      active_list_add(_queue_lookahead[i]);
    }
    _queue_lookahead_size = 0;
  }
}

unsigned int instr_queue_free_int_spots_next_clock() {
  return min(INT_QUEUE_SIZE - get_queue_size(int_queue_head) + functional_free_int_spots_next_clock(), INT_QUEUE_SIZE);
}

unsigned int instr_queue_free_addr_spots_next_clock() {
  return min(ADDR_QUEUE_SIZE - get_queue_size(addr_queue_head) + how_many_addr_will_commit_next_clock(), ADDR_QUEUE_SIZE);
}


void instr_queue_handle_mispredict_helper(unsigned int addr, instr_queue_entry** queue) {
  if(NULL == *queue) return;
  instr_queue_entry* current = *queue;
  instr_queue_entry* prev;
  while(NULL != current) {
    if(current->instruction->addr > addr) {
      if(*queue == current) {
	*queue = current->next;
      } else {
	prev->next = current->next;
      }
      free(current);
      current = current->next;
    } else {
      prev = current;
      current = current->next;
    }
  }
}

void instr_queue_handle_mispredict(instr* instruction) {
  instr_queue_handle_mispredict_helper(instruction->addr, &int_queue_head);
  instr_queue_handle_mispredict_helper(instruction->addr, &addr_queue_head);
  instr_queue_handle_mispredict_helper(instruction->addr, &fp_queue_head);
}
