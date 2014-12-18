#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include "backend.h"
#include "instr_queue.h"
#include "instr.h"
#include "constants.h"

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
