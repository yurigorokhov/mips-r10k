#ifndef BACKEND_H
#define BACKEND_H

#include "error.h"
#include "frontend.h"
#include "instr_queue.h"
#include "reg_map.h"
#include "active_list.h"

#ifndef BACKEND_DISPATCH_PER_CYCLE
#define BACKEND_DISPATCH_PER_CYCLE 4
#endif

error_code backend_step();

#endif
