/* This file sets up the parameters for the MIPS R10K */

/*---- Width of CPU ----*/

/* Note this affects fetch / decode / issue and commit */
#ifndef BACKEND_DISPATCH_PER_CYCLE
#define BACKEND_DISPATCH_PER_CYCLE 4
#endif

/*--- Registers ----*/
/* It is assumed there are 2x phys regs as logi regs  *
 * and as many logi regs as the active list           */
#ifndef PHYS_REGS
#define PHYS_REGS 64
#endif

#ifndef LOGI_REGS
#define LOGI_REGS 32
#endif

/*---- Buffer sizes ----*/
#ifndef ACTIVE_LIST_SIZE
#define ACTIVE_LIST_SIZE 32
#endif

#ifndef DECODE_BUFFER_SIZE
#define DECODE_BUFFER_SIZE 8
#endif

#ifndef INT_QUEUE_SIZE
#define INT_QUEUE_SIZE 16
#endif

#ifndef ADDR_QUEUE_SIZE
#define ADDR_QUEUE_SIZE 16
#endif

#ifndef FP_QUEUE_SIZE
#define FP_QUEUE_SIZE 16
#endif

#ifndef BRANCH_STACK_SIZE
#define BRANCH_STACK_SIZE 4
#endif

/*---- Cycle times ----*/
/*---- (These are not meant to be changed, behavior unpredictable) ----*/

#ifndef FETCH_CYCLES
#define FETCH_CYCLES 1
#endif

#ifndef DECODE_CYCLES
#define DECODE_CYCLES 1
#endif

#ifndef ISSUE_CYCLES
#define ISSUE_CYCLES 1
#endif

#ifndef FUNCTIONAL_INTEGER_CYCLES
#define FUNCTIONAL_INTEGER_CYCLES 1
#endif

#ifndef FUNCTIONAL_FP_CYCLES
#define FUNCTIONAL_FP_CYCLES 3
#endif

#ifndef FUNCTIONAL_LOAD_STORE_CYCLES
#define FUNCTIONAL_LOAD_STORE_CYCLES 1
#endif

