#ifndef FRONTEND_H
#define FRONTEND_H

#include "instr.h"
#include "error.h"

void frontend_init(unsigned int);
result frontend_enqueue(instr*);
void frontend_clean();

#endif
