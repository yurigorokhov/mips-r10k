#ifndef DECODE_BUFFER_H
#define DECODE_BUFFER_H

#include <stdbool.h>
#include "error.h"
#include "instr.h"
#include "misc.h"
#include "fetch_stage.h"
#include "constants.h"
#include "branch_stack.h"

typedef struct {
  instr* instruction;
  unsigned int cycles_left;
  void* next;
} decode_buffer_entry;

void __calc_decode_buffer();
void __edge_decode_buffer();

instr* decode_buffer_get_next_ready_instr(unsigned int skip);
void decode_buffer_remove_instruction(instr* instruction);

void decode_buffer_handle_mispredict();

#endif
