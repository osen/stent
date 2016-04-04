#ifndef STENTEXT_H
#define STENTEXT_H

#include "stent.h"

size_t _AbortIfNotLess(size_t a, size_t b);

#define ARRAY(T) \
  struct _##T##ArrayRef

#define ARRAY_ALLOC(T) \
  *((struct _##T##ArrayRef*)_RefCalloc(sizeof(struct _##T##Array), "struct "#T"[]", __FILE__, __LINE__))

#define ARRAY_FREE(A) \
  do \
  { \
    if(GET(A) == NULL) break; \
    if(GET(A)->data) free(GET(A)->data); \
    FREE(A); \
  } \
  while(0)

#define ARRAY_ADD(A, E) \
  do \
  { \
    void *tmp = NULL; \
    tmp = realloc(GET(A)->data, sizeof(*GET(A)->data) * (GET(A)->size + 1)); \
    if(tmp == NULL) abort(); \
    GET(A)->data = tmp; \
    GET(A)->data[GET(A)->size] = E; \
    GET(A)->size++; \
  } \
  while(0)

#define ARRAY_SIZE(A) \
  GET(A)->size

#define ARRAY_AT(A, I) \
  GET(A)->data[_AbortIfNotLess(I, GET(A)->size)]

REFDEF(String);

REF(String) StringAllocCStr(char *str);
void StringFree(REF(String) ctx);
char *StringCStr(REF(String) ctx);

#endif
