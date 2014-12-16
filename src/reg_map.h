#ifndef REGMAP_H
#define REGMAP_H

#include <limits.h>
#include <stdbool.h>
#include "instr.h"

#define PHYS_REGS 64
#define LOGI_REGS 32

typedef unsigned int phys_reg;
typedef unsigned int logi_reg;

typedef struct {
  phys_reg physical;
  void* next;
} free_list_entry;

typedef struct {
  phys_reg physical;
  logi_reg logical;
  instr* instruction_writing;
  void* next;
} busy_list_entry;

//--- Functions ---
void reg_map_init();
phys_reg reg_map_assign(instr*);
instr* reg_map_get_latest_instr_writing_to(logi_reg, unsigned int);
void reg_map_free_by_logical(logi_reg);

#endif
