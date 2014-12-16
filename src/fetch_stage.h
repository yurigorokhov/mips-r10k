#ifndef FETCH_STAGE_H
#define FETCH_STAGE_H

#include "instr.h"
#include "backend.h"

#define FETCH_CYCLES 1

typedef struct {
  instr* instruction;
  unsigned int cycles_left;
  void* next;
} fetch_stage_entry;

/*
 * returns int - how many instructions can be fetched on next clock
 */
unsigned int __calc_fetch_stage();
void __edge_fetch_stage_add_instr(instr*);
instr* fetch_get_ready_instr(unsigned int);
void fetch_stage_remove_instr(instr*);

#endif
