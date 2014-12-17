#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include "backend.h"
#include "instr_queue.h"
#include "instr.h"

#define FUNCTIONAL_INTEGER_CYCLES 1
#define FUNCTIONAL_FP_CYCLES 3
#define FUNCTIONAL_LOAD_STORE_CYCLES 2

typedef struct {
  instr* instruction;
  unsigned int cycles_left;
  void* next;
} functional_entry;

void __calc_functional_units();
void __edge_functional_units();

unsigned int f_taking_int();
unsigned int f_taking_fp();
unsigned int f_taking_addr();

#endif
