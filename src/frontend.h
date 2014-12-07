#ifndef FRONTEND_H
#define FRONTEND_H

#include "instr.h"
#include <stdlib.h>

void frontend_init(unsigned int);
void frontend_enqueue(instr*);
void frontend_clean();
void frontend_getinstr(unsigned int, instr**);

#endif
