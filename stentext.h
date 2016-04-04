#ifndef STENTEXT_H
#define STENTEXT_H

#include "stent.h"

REFDEF(String);

REF(String) StringAllocCStr(char *str);
void StringFree(REF(String) ctx);
char *StringCStr(REF(String) ctx);

#endif
