#ifndef INSTR_QUEUE_H
#define INSTR_QUEUE_H

#include <limits.h>
#include "instr.h"
#include "error.h"

#define INT_QUEUE_SIZE 16
#define ADDR_QUEUE_SIZE 16
#define FP_QUEUE_SIZE 16

char instr_queue_is_full(instr *);
error_code instr_queue_add(instr*);
void instr_queue_remove(instr *);

#endif
