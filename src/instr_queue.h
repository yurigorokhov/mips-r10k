#ifndef INSTR_QUEUE_H
#define INSTR_QUEUE_H

#include <limits.h>
#include <stdbool.h>
#include "instr.h"
#include "error.h"
#include "backend.h"
#include "active_list.h"
#include "misc.h"
#include "committer.h"

#define INT_QUEUE_SIZE 16
#define ADDR_QUEUE_SIZE 16
#define FP_QUEUE_SIZE 16

#define ISSUE_CYCLES 1

typedef struct {
  instr* instruction;
  unsigned int cycles_left;
  void* next;
} instr_queue_entry;

void __calc_instr_queue();
void __edge_instr_queue();
void instr_queue_remove(instr *);

instr* instr_queue_get_ready_int_instr(unsigned int skip, bool is_alu_1);
instr* instr_queue_get_ready_addr_instr();
instr* instr_queue_get_ready_fp_instr(bool);

unsigned int instr_queue_free_int_spots_next_clock();
unsigned int instr_queue_free_addr_spots_next_clock();
unsigned int instr_queue_free_fp_spots_next_clock();

void instr_queue_handle_mispredict(instr*);

#endif
