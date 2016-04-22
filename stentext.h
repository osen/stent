#ifndef STENTEXT_H
#define STENTEXT_H

#include "stent.h"

REFDEF(String);

REF(String) StringAllocCStr(char *str);
char *StringCStr(REF(String) ctx);
void StringAddChar(REF(String) ctx, char c);
void StringClear(REF(String) ctx);
void StringCopy(REF(String) dest, REF(String) src);
void StringAdd(REF(String) ctx, REF(String) add);
size_t StringLength(REF(String) ctx);
void StringAddCStr(REF(String) ctx, char *add);
void StringAddInt(REF(String) ctx, int add);

REFDEF(InputFile);

REF(InputFile) InputFileOpen(char *path);
int InputFileEof(REF(InputFile) ctx);
void InputFileReadLine(REF(InputFile) ctx, REF(String) out);

#endif
