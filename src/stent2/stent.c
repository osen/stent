#include "stent.h"

#include <stdio.h>

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

void _sfree(void **ptr, char *file, size_t line)
{
  struct _StentAllocation *allocation = NULL;

  _sinit();
  _svalid(ptr, file, line);

  allocation = (struct _StentAllocation *)ptr;
  free(allocation->ptr);
  allocation->expired = 1;
}

int _svalid(void **ptr, char *file, size_t line)
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

