#ifndef BACKEND_H
#define BACKEND_H

#include "error.h"
#include "frontend.h"
#include "instr_queue.h"
#include "reg_map.h"
#include "active_list.h"
#include "decode_buffer.h"
#include "functional.h"

#ifndef BACKEND_DISPATCH_PER_CYCLE
#define BACKEND_DISPATCH_PER_CYCLE 4
#endif

error_code backend_cycle_step();

#endif
