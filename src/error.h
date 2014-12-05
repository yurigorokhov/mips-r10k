#ifndef ERROR_H
#define ERROR_H

typedef enum { SUCCESS, ERROR } err;
typedef struct {
  err status;
  const char* errorMsg;
} result;

//--- Functions ---
result get_success();
result get_error(const char* str);

#endif
