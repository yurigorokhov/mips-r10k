#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include "instr_queue.h"
#include "instr.h"

typedef struct {
  instr* instruction;
} functional_entry;

void __calc_functional_units();
void __edge_functional_units();

#endif
