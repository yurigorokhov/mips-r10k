#ifndef ACTIVE_LIST_H
#define ACTIVE_LIST_H

#include "error.h"
#include "instr.h"

#define ACTIVE_LIST_SIZE 32

typedef struct {
  instr* instruction;
  void* next;
} active_list_entry;

//--- Functions ---
char active_list_is_full();
char active_list_is_empty();
error_code active_list_add(instr* instruction);

#endif
