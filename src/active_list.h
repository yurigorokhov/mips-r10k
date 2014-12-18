#ifndef ACTIVE_LIST_H
#define ACTIVE_LIST_H

#include <stdbool.h>
#include "error.h"
#include "instr.h"
#include "committer.h"
#include "misc.h"
#include "reg_map.h"
#include "branch_stack.h"
#include "constants.h"

typedef struct {
  instr* instruction;
  char is_ready_on_next_clock;
  bool is_res_ready_on_next_clock;
  bool has_done_mem_cycle;
  phys_reg physical;
  void* next;
} active_list_entry;

//--- Functions ---
char active_list_is_full();
char active_list_is_empty();
error_code active_list_add(instr* instruction);
instr* active_list_get_instr_ready(unsigned int skip, bool*);
void active_list_set_instr_ready(instr* instruction);
void active_list_set_instr_res_ready(instr* instruction);
void active_list_commit_instruction();
unsigned int active_list_how_many_spots_next_clock();
bool active_list_is_instr_ready(instr*);
void active_list_handle_mispredict(instr*);
unsigned int num_active_branches();
void active_list_set_mem_cycle(instr*);

#endif
