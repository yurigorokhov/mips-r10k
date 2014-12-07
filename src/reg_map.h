#ifndef REGMAP_H
#define REGMAP_H

#include <limits.h>
#include "error.h"
#include "instr.h"

#define PHYS_REGS 64
#define LOGI_REGS 32

//--- Functions ---
error_code map_registers_for_instruction(instr *);
int get_phys_reg_mapping(uint arch_reg);
char reg_map_is_full();

#endif
