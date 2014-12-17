#ifndef FRONTEND_H
#define FRONTEND_H

#include "instr.h"
#include <stdlib.h>

void frontend_init(unsigned int);
void frontend_enqueue(instr*);
void frontend_clean();
error_code frontend_getinstr(unsigned int, instr**);
error_code frontend_getinstr_with_step(unsigned int step, instr** res);
unsigned int frontend_get_instr_count();
unsigned int frontend_get_max_step();
unsigned int frontend_augment_steps_starting_at_addr(unsigned int);

#endif
