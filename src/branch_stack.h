#ifndef BRANCH_STACK
#define BRANCH_STACK

#include <stdbool.h>
#include "instr.h"

typedef struct {
  instr* instruction;
  void* next;
  bool is_ready_on_next_clock;
} bs_entry;

void bs_add(instr*);
void bs_remove(instr*);
void bs_set_instr_ready(instr*);
unsigned int bs_size_next_clock();

#endif
