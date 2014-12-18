bin_PROGRAMS = mipsr10k

AM_CXXFLAGS = $(INTI_CFLAGS)

mipsr10k_SOURCES = mipsr10k.c instr.c instr.h error.c error.h frontend.c frontend.h backend.c backend.h reg_map.c reg_map.h instr_queue.c instr_queue.h active_list.c active_list.h decode_buffer.h decode_buffer.c functional.h functional.c committer.h committer.c misc.h misc.c fetch_stage.h fetch_stage.c constants.h branch_stack.h branch_stack.c
mipsr10k_LDADD = $(INTI_LIBS)

if TEST_FILE_PROVIDED
TEST_FILE = test/s1
endif

all-local: mipsr10k$(EXEEXT)
	./mipsr10k ../$(TEST_FILE)

