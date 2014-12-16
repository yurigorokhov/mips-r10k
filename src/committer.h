#ifndef COMMITTER_H
#define COMMITTER_H

#include "backend.h"
#include "instr_queue.h"

void __calc_committer();
void __edge_committer();
unsigned int how_many_will_commit_next_clock();
unsigned int how_many_addr_will_commit_next_clock();
#endif
