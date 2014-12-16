#ifndef FRONTEND_H
#define FRONTEND_H

#include "instr.h"
#include <stdlib.h>

void frontend_init(unsigned int);
void frontend_enqueue(instr*);
void frontend_clean();
error_code frontend_getinstr(unsigned int, instr**);
unsigned int frontend_get_instr_count();

#endif
