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
  const char *type;
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
    _sblocks = (struct _StentBlock *)calloc(1, sizeof(*_sblocks));
  }
}

ref(void) _salloc(size_t size, const char *type, void *placement)
{
  ref(void) rtn = NULL;
  struct _StentBlock *sb = NULL;

  _sinit();

  rtn = (ref(void))&_sblocks->allocations[_sblocks->count];

  if(size)
  {
    rtn[0] = calloc(1, size);

    if(!rtn[0])
    {
      fprintf(stderr, "Error: Failed to allocate %s\n", type);
      abort();
    }
  }
  else
  {
    if(!placement)
    {
      fprintf(stderr, "Error: No size specified and no placement data %s\n",
        type);

      abort();
    }

    rtn[0] = placement;
  }

  _sblocks->allocations[_sblocks->count].type = type;
  _sblocks->count++;

  if(_sblocks->count >= STENT_BLOCKSIZE)
  {
    fprintf(stderr, "Warning: Adding allocation blocks\n");
    sb = (struct _StentBlock *)calloc(1, sizeof(*_sblocks));
    sb->next = _sblocks;
    _sblocks = sb;
  }

  return rtn;
}

void _sfree(ref(void) ptr, const char *file, size_t line)
{
  struct _StentAllocation *allocation = NULL;

  _sinit();
  _svalid(ptr, file, line);

  allocation = (struct _StentAllocation *)ptr;
  free(allocation->ptr);
  allocation->expired = 1;
}

int _svalid(ref(void) ptr, const char *file, size_t line)
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

#endif

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

#ifdef STENT_ENABLE
vector(void) _vector_new(size_t size, const char *type)
#else
vector(void) _vector_new(size_t size)
#endif
{
  ref(struct _StentVector) rtn = NULL;

#ifdef STENT_ENABLE
  rtn = (ref(struct _StentVector))_salloc(sizeof(struct _StentVector),
    type, NULL);
#else
  rtn = salloc(struct _StentVector);
#endif

  _(rtn)->elementSize = size;

  return (vector(void))rtn;
}

#ifdef STENT_ENABLE
void _vector_delete(vector(void) ptr, const char *file, size_t line)
#else
void _vector_delete(vector(void) ptr)
#endif
{
  ref(struct _StentVector) v = NULL;

  v = (ref(struct _StentVector))ptr;
  free(_(v)->data);

#ifdef STENT_ENABLE
  _sfree((void **)ptr, file, line);
#else
  sfree((void **)ptr);
#endif
}

size_t _vector_size(vector(void) ptr)
{
  ref(struct _StentVector) v = NULL;

  v = (ref(struct _StentVector))ptr;

  return _(v)->size;
}

void _vector_resize(vector(void) ptr, size_t size)
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

size_t _vector_valid(vector(void) ptr, size_t idx)
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
