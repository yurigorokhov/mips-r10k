#include <stdlib.h>
#include "error.h"

result get_success() {
  result res = { SUCCESS, "" };
  return res;
};

result get_error(const char* errorMsg) {
  result res = { ERROR, errorMsg };
  return res;
};
