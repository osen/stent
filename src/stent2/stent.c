#include "stent.h"

#include <stdio.h>
#include <string.h>

#ifdef STENT_ENABLE

/***************************************************
 * Reference
 ***************************************************/

struct _StentAllocation
{
  void *ptr;
  int expired;
  char *type;
};

struct _StentBlock
{
  struct _StentAllocation allocations[STENT_BLOCKSIZE];
  size_t count;
  struct _StentBlock *next;
};

struct _StentBlock *_sblocks;

void _sinit()
{
  if(!_sblocks)
  {
    _sblocks = calloc(1, sizeof(*_sblocks));
  }
}

void **_salloc(size_t size, char *type)
{
  ref(void) rtn = NULL;
  struct _StentBlock *sb = NULL;

  _sinit();

  rtn = (ref(void))&_sblocks->allocations[_sblocks->count];
  rtn[0] = calloc(1, size);

  if(!rtn[0])
  {
    fprintf(stderr, "Error: Failed to allocate %s\n", type);
    abort();
  }

  _sblocks->allocations[_sblocks->count].type = type;
  _sblocks->count++;

  if(_sblocks->count >= STENT_BLOCKSIZE)
  {
    fprintf(stderr, "Warning: Adding allocation blocks\n");
    sb = calloc(1, sizeof(*_sblocks));
    sb->next = _sblocks;
    _sblocks = sb;
  }

  return rtn;
}

void _sfree(void **ptr, char *file, size_t line, int dummy)
{
  struct _StentAllocation *allocation = NULL;

  _sinit();
  _svalid(ptr, file, line, 0);

  allocation = (struct _StentAllocation *)ptr;
  free(allocation->ptr);
  allocation->expired = 1;
}

int _svalid(void **ptr, char *file, size_t line, int dummy)
{
  struct _StentAllocation *allocation = NULL;

  _sinit();
  allocation = (struct _StentAllocation *)ptr;

  if(allocation->expired)
  {
    fprintf(stderr, "Error: %s pointer no longer valid in %s %i\n",
      allocation->type, file, (int)line);

    abort();
  }

  return 1;
}

/***************************************************
 * Vector
 ***************************************************/

struct _StentVector
{
  void *data;
  size_t size;
  size_t allocated;
  size_t elementSize;
};

void ***_vector_new(size_t size, char *type)
{
  ref(struct _StentVector) rtn = NULL;

  rtn = (ref(struct _StentVector))_salloc(sizeof(struct _StentVector), type);
  _(rtn)->elementSize = size;

  return (void ***)rtn;
}

void _vector_delete(void ***ptr, char *file, size_t line, int dummy)
{
  ref(struct _StentVector) v = NULL;

  v = (ref(struct _StentVector))ptr;
  free(_(v)->data);

  _sfree((void **)ptr, file, line, 0);
}

size_t _vector_size(void ***ptr, int dummy)
{
  ref(struct _StentVector) v = NULL;

  v = (ref(struct _StentVector))ptr;

  return _(v)->size;
}

void _vector_resize(void ***ptr, size_t size, int dummy)
{
  ref(struct _StentVector) v = NULL;
  size_t s = 0;
  void *d = NULL;

  v = (ref(struct _StentVector))ptr;

  if(_(v)->allocated >= size)
  {
    _(v)->size = size;
    return;
  }

  s = 1;

  while(1)
  {
    if(s >= size)
    {
      break;
    }

    s = s * 2;
  }

  d = calloc(s, _(v)->elementSize);

  if(!d)
  {
    fprintf(stderr, "Error: Failed to increase vector size\n");
    abort();
  }

  memcpy(d, _(v)->data, _(v)->elementSize * _(v)->size);
  free(_(v)->data);
  _(v)->data = d;
  _(v)->allocated = s;
  _(v)->size = size;
}

size_t _vector_valid(void ***ptr, size_t idx)
{
  ref(struct _StentVector) v = NULL;

  v = (ref(struct _StentVector))ptr;

  if(_(v)->size > idx)
  {
    return idx;
  }

  fprintf(stderr, "Error: Index out of bounds\n");
  abort();
}

#endif
