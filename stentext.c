#include "stentext.h"

#include <string.h>

struct String
{
  char *data;
  size_t length;
};

REF(String) StringAllocCStr(char *str)
{
  REF(String) rtn = {};

  rtn = CALLOC(String);
  GET(rtn)->length = strlen(str);
  GET(rtn)->data = calloc(GET(rtn)->length + 1, sizeof(char));

#ifdef __OpenBSD__
  strlcpy(GET(rtn)->data, str, GET(rtn)->length + 1);
#else
  strcpy(GET(rtn)->data, str);
#endif

  return rtn;
}

void StringFree(REF(String) ctx)
{
  free(GET(ctx)->data);
  FREE(ctx);
}

char *StringCStr(REF(String) ctx)
{
  return GET(ctx)->data;
}
