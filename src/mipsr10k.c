#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "instr.h"
#include "error.h"
#include "frontend.h"
#include "backend.h"
#include "reg_map.h"

void print_history(unsigned int);

int main(int argc, char **argv) {
  if(argc != 2) {
    printf("\nUSAGE: mips filename\n");
    return 1;
  }
  const char* filename = argv[1];
  if(0 != access(filename, R_OK)) {
    printf("\nCannot read file: %s\n", filename);
    return 1;
  }
  error_code code;

  // read file and parse instructions into frontend
  frontend_init(100);
  char line_buffer[80];
  FILE* input_file = fopen(filename, "r");
  unsigned int i = 1;
  while(fgets(line_buffer, 80, input_file)) {
    
    // remove new line
    unsigned int j = 0;
    while(line_buffer[j] != '\n') { j++; }
    line_buffer[j] = '\0';
    
    char* strcpy = malloc(sizeof(char)*(j+1));
    memcpy(strcpy, line_buffer, j+1);

    // parse instruction
    instr* parsed_instr;
    code = parse_instruction(strcpy, &parsed_instr, i);
    if(SUCCESS != code) {
      printf("%s: %i (%s)", get_error(code), i, line_buffer);
      exit(1);
    }
    
    // store the instruction in the frontend
    frontend_enqueue(parsed_instr);
    ++i;
  }
  printf("\nLoaded %i instructions\n", i-1);
  fclose(input_file);

  // run simulation
  reg_map_init();
  while(SUCCESS == (code = backend_cycle_step()));
  frontend_clean();
  print_history(backend_get_cycle());
  return 0;
}

void print_history(unsigned int num_cycles) {
  history_step* history_current = get_history();

  // write header
  unsigned int i,  j;
  printf("|%7s|%20s|", "addr", "instruction");
  for(i = 1; i <= num_cycles; ++i) {
    printf("%2i|", i);
  }
  printf("\n");
  
  // write instructions
  instr_stage prev_stage = NONE;
  while(NULL != history_current) {
    instr_history_entry* current = history_current->entry;

    // print the address & instruction
    printf("|0x%5.5x|%20s|",
	   current->instruction->addr, current->instruction->original_str); /** TODO branch extra */
    for(j = 1; j <= num_cycles; j++) {
      if(NULL == current || current->cycle_num != j) {
	printf("%2s|", "");
	continue;
      }
      instr_stage stage = current->instruction->stage;
      if(stage == prev_stage) {
	printf("%2s|", "");
      } else {
	printf("%2s|", get_str(current->instruction->stage, current->instruction->op));
      }
      prev_stage = stage;
      current = current->next;
    }
    printf("\n");
    history_current = history_current->next;
  }
}
