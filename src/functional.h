#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include "backend.h"
#include "instr_queue.h"
#include "instr.h"

#define FUNCTIONAL_INTEGER_CYCLES 1
#define FUNCTIONAL_FP_CYCLES 2

typedef struct {
  instr* instruction;
  unsigned int cycles_left;
} functional_entry;

void __calc_functional_units();
void __edge_functional_units();
unsigned int functional_free_int_spots_next_clock();
unsigned int function_free_fp_next_clock();

#endif
