#ifndef DECODE_BUFFER_H
#define DECODE_BUFFER_H

#include "error.h"
#include "instr.h"
#include "misc.h"

#define DECODE_BUFFER_SIZE 8
#define FETCH_CYCLES 1

typedef struct {
  instr* instruction;
  unsigned int cycles_left;
  void* next;
} decode_buffer_entry;

/*
 * Schedule instruction to be added to decode stage, next clock
 * Note that it may not be added due to a full buffer
 */
error_code __calc_decode_buffer_add(instr*);

/*
 * Instructions will be accepted in order provided
 * returns int - number of instructions accepted
 */
int __edge_decode_buffer_accept_instructions();

instr* decode_buffer_get_next_ready_instr(unsigned int skip);
void decode_buffer_remove_instruction(instr* instruction);

#endif
