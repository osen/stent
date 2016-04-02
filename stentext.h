#ifndef STENTEXT_H
#define STENTEXT_H

#include "stent.h"

#define ARRAY(T) \
  struct _##T##ArrayRef

#define ARRAY_ALLOC(T) \
  *((struct _##T##ArrayRef*)_RefCalloc(sizeof(struct _##T##Array), "struct "#T"[]", __FILE__, __LINE__))

#define ARRAY_FREE(A) \
  if(GET(A)->data) free(GET(A)->data); \
  FREE(A)

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
  GET(A)->data[I]

#endif
