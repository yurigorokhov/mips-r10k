#ifndef INSTR_QUEUE_H
#define INSTR_QUEUE_H

#include <limits.h>
#include "instr.h"
#include "error.h"
#include "backend.h"

#define INT_QUEUE_SIZE 16
#define ADDR_QUEUE_SIZE 16
#define FP_QUEUE_SIZE 16

typedef struct {
  instr* instruction;
  void* next;
} instr_queue_entry;

void __calc_instr_queue();
void __edge_instr_queue();
void instr_queue_remove(instr *);
instr* instr_queue_get_ready_int_instr(unsigned int skip);


#endif
