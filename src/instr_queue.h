#ifndef INSTR_QUEUE_H
#define INSTR_QUEUE_H

#include <limits.h>
#include "instr.h"
#include "error.h"
#include "backend.h"
#include "active_list.h"
#include "misc.h"

#define INT_QUEUE_SIZE 16
#define ADDR_QUEUE_SIZE 16
#define FP_QUEUE_SIZE 16

#define DECODE_CYCLES 1

typedef struct {
  instr* instruction;
  unsigned int cycles_left;
  void* next;
} instr_queue_entry;

void __calc_instr_queue();
void __edge_instr_queue();
void instr_queue_remove(instr *);
instr* instr_queue_get_ready_int_instr(unsigned int skip);
unsigned int instr_queue_free_int_spots_next_clock();

#endif
