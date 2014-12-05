#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "instr.h"
#include "error.h"
#include "frontend.h"

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

  // read file and parse instructions into frontend
  frontend_init(100);
  char line_buffer[80];
  FILE* input_file = fopen(filename, "r");
  unsigned int i = 1;
  while(fgets(line_buffer, 80, input_file)) {
    
    // parse instruction
    instr* parsed_instr;
    result parse_res = parse_instruction(line_buffer, &parsed_instr);
    if(parse_res.status == ERROR) {
      printf("%s: %i (%s)", parse_res.errorMsg, i, line_buffer);
      exit(1);
    }

    // store the instruction in the frontend
    frontend_enqueue(parsed_instr);
    ++i;
  }
  printf("\nLoaded %i instructions\n", i);
  fclose(input_file);
  frontend_clean();
  return 0;
}
