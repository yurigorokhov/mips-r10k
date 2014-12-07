#include "reg_map.h"

static unsigned int PHYS_REGS_VALS[PHYS_REGS];

//TODO: may be better to inverse this mapping
static unsigned int REG_MAP[PHYS_REGS] = { UINT_MAX };

//--- Functions ---
int get_free_phys_register() {
  
  //TODO: is reg0 valid?
  int i;
  for(i = 0; i < PHYS_REGS; ++i) {
    if(UINT_MAX != REG_MAP[i]) {
      return i;
    }
  }
  return -1;
}

error_code map_registers_for_instruction(instr *instruction) {
  
  // if this instruction has a destination, we need to map it to a reg
  if(instruction->rd != UINT_MAX) {
    int free_phys_reg = get_free_phys_register();
    if(-1 == free_phys_reg) {
      return REGISTER_MAP_FULL;
    }
    
    // map the free physical register to the instructions arch register
    REG_MAP[free_phys_reg] = instruction->rd;
  }
  return SUCCESS;
}

int get_phys_reg_mapping(uint arch_reg) {
  int i;
  for(i = 0; i < PHYS_REGS; i++) {
    if(arch_reg == REG_MAP[i]) {
      return i;
    }
  }
  return -1;
}

char reg_map_is_full() {
  if(-1 == get_free_phys_register()) {
    return 1;
  }
  return 0;
}
