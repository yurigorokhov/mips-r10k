#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "instr.h"
#include "error.h"
#include "frontend.h"
#include "backend.h"

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
    
    // parse instruction
    instr* parsed_instr;
    code = parse_instruction(line_buffer, &parsed_instr, i);
    if(SUCCESS != code) {
      printf("%s: %i (%s)", get_error(code), i, line_buffer);
      exit(1);
    }

    // store the instruction in the frontend
    frontend_enqueue(parsed_instr);
    ++i;
  }
  printf("\nLoaded %i instructions\n", i);
  fclose(input_file);
  while(SUCCESS == (code = backend_step())) {
    // cycle has happened
  }
  frontend_clean();
  return 0;
}
