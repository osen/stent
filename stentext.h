#ifndef STENTEXT_H
#define STENTEXT_H

#include "stent.h"

#define ARRAY(T) \
  struct _##T##ArrayRef

#define ARRAY_ALLOC(T) \
  *((struct _##T##ArrayRef*)_RefCalloc(sizeof(struct _##T##Array), "struct "#T"[]", __FILE__, __LINE__))

#endif
