#include "instr_queue.h"

// integer queue
static instr* INTEGER_QUEUE[INT_QUEUE_SIZE];

// address queue
static instr* ADDRESS_QUEUE[ADDR_QUEUE_SIZE];

// floating point queue
static instr* FP_QUEUE[FP_QUEUE_SIZE];

//--- Functions ---
int get_queue_free_spot(instr** queue, size_t size) {
  int i;
  for(i = 0; i < size; i++) {
    if(NULL == queue[i]) {
      return i;
    }
  }
  return -1;
}

int get_int_queue_free_spot() {
  return get_queue_free_spot(INTEGER_QUEUE, INT_QUEUE_SIZE);
}

int get_addr_queue_free_spot() {
  return get_queue_free_spot(ADDRESS_QUEUE, ADDR_QUEUE_SIZE);
}

int get_fp_queue_free_spot() {
  return get_queue_free_spot(FP_QUEUE, FP_QUEUE_SIZE);
}

error_code instr_queue_add(instr* instruction) {
  int free_spot;
  switch(instruction->op) {
  case INTEGER:
   
    // check if queue is full
    free_spot = get_int_queue_free_spot();
    if(-1 == free_spot) {
      return INSTR_QUEUE_FULL;
    }

    // add to queue
    INTEGER_QUEUE[free_spot] = instruction;
    break;
  case LOAD:
  case STORE:
    
    // check if queue is full
    free_spot = get_addr_queue_free_spot();
    if(-1 == free_spot) {
      return INSTR_QUEUE_FULL;
    }

    // add to queue
    ADDRESS_QUEUE[free_spot] = instruction;
    break;
  case BRANCH:

    //TODO: needs special handling
    break;
  case FPADD:
  case FPMUL:
    
    // check if queue is full
    free_spot = get_fp_queue_free_spot();
    if(-1 == free_spot) {
      return INSTR_QUEUE_FULL;
    }

    // add to queue
    FP_QUEUE[free_spot] = instruction;
    break;
  }
  return SUCCESS;
}
