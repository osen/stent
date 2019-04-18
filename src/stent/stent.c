#include "stent.h"

#include <stdio.h>
#include <string.h>
#include <setjmp.h>

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

static struct _StentBlock *_sblocks;

static void _satexit()
{
  struct _StentBlock *sb = NULL;
  size_t ai = 0;

  sb = _sblocks;

  while(sb)
  {
    for(ai = 0; ai < sb->count; ai++)
    {
      if(!sb->allocations[ai].expired)
      {
        fprintf(stderr,
          "Warning: Allocated memory persisted after application exit [%s]\n",
          sb->allocations[ai].type);
      }
    }

    sb = sb->next;
  }
}

static void _sinit()
{
  if(!_sblocks)
  {
    _sblocks = (struct _StentBlock *)calloc(1, sizeof(*_sblocks));

    if(!_sblocks)
    {
      fprintf(stderr, "Error: Failed to initialize initial block\n");

      abort();
    }

    fprintf(stderr, "Warning: Debug memory allocator enabled\n");
    atexit(_satexit);
  }
}

ref(void) _salloc(size_t size, const char *type, void *placement)
{
  ref(void) rtn = NULL;
  struct _StentBlock *sb = NULL;

  _sinit();

  if(!size && !placement)
  {
    return NULL;
  }

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

  if(!ptr)
  {
    fprintf(stderr, "Error: NULL pointer in %s %i\n",
      file, (int)line);
  }

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
  _sfree((ref(void))ptr, file, line);
#else
  sfree(ptr);
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

void _vector_erase(vector(void) ptr, size_t idx, size_t num)
{
  ref(struct _StentVector) v = NULL;
  char *dest = NULL;
  char *src = NULL;
  size_t tm = 0;

  v = (ref(struct _StentVector))ptr;

  if(idx >= _(v)->size ||
    idx + num > _(v)->size)
  {
    fprintf(stderr, "Error: Index out of bounds\n");
    abort();
  }

  if(!num)
  {
    return;
  }

  dest = (char *)_(v)->data;
  dest += (idx * _(v)->elementSize);

  src = dest;
  src += (num * _(v)->elementSize);

  tm = (_(v)->size - (idx + num)) * _(v)->elementSize;

  if(tm)
  {
    memmove(dest, src, tm);
  }

  _(v)->size -= num;
}

void _vector_insert(vector(void) ptr, size_t before,
  vector(void) source, size_t idx, size_t num)
{
  ref(struct _StentVector) s = NULL;
  ref(struct _StentVector) d = NULL;
  char *dest = NULL;
  char *src = NULL;
  size_t tm = 0;

  s = (ref(struct _StentVector))source;
  d = (ref(struct _StentVector))ptr;

  if(s == d)
  {
    fprintf(stderr, "Error: Source and desination must not match\n");
    abort();
  }

  if(!num)
  {
    return;
  }

  if(before > _(d)->size)
  {
    fprintf(stderr, "Error: Invalid index specified. Non contiguous\n");
    abort();
  }

  if(idx >= _(s)->size ||
    idx + num > _(s)->size)
  {
    fprintf(stderr, "Error: Index out of bounds on source\n");
    abort();
  }

  tm = (_(d)->size - before) * _(d)->elementSize;

  _vector_resize(ptr, _(d)->size + num);

  src = (char *)_(d)->data;
  src += (before * _(d)->elementSize);
  dest = src;
  dest += (num * _(d)->elementSize);
  memmove(dest, src, tm);

  dest = src;
  src = (char *)_(s)->data;
  src += (idx * _(d)->elementSize);
  memcpy(dest, src, num * _(s)->elementSize);
}

/***************************************************
 * Exceptions
 ***************************************************/

struct _StentCatch
{
  jmp_buf buf;
  struct Exception ex;
  struct _StentCatch *next;
};

static struct _StentCatch *_scatchs;
static size_t _scatchDepth;

static struct _StentCatch *_StentCatchAtDepth(size_t depth)
{
  struct _StentCatch *sc = NULL;

  if(!_scatchs)
  {
    _scatchs = (struct _StentCatch *)calloc(1, sizeof(*_scatchs));
  }

  sc = _scatchs;

  while(depth)
  {
    if(!sc->next)
    {
      sc->next = (struct _StentCatch *)calloc(1, sizeof(*_scatchs));
    }

    sc = sc->next;
    depth--;
  }

  return sc;
}

void sthrow(int type, const char *message)
{
  struct _StentCatch *sc = NULL;
  size_t ml = 0;

  if(!_scatchDepth)
  {
    fprintf(stderr, "Error: Unhandled exception [%i] %s\n", type, message);
    abort();
  }

  sc = _StentCatchAtDepth(_scatchDepth - 1);
  sc->ex.type = type;

  ml = strlen(message);

  if(ml >= STENT_EXCEPTION_MESSAGE_LENGTH)
  {
    ml = STENT_EXCEPTION_MESSAGE_LENGTH - 1;
  }

  strncpy(sc->ex.message, message, ml);
  sc->ex.message[ml] = '\0';

  longjmp(sc->buf, 1);
}

int _scatch(struct Exception *ex, void (*func)(ref(void)), ref(void) ptr)
{
  struct _StentCatch *sc = NULL;

  _scatchDepth ++;
  sc = _StentCatchAtDepth(_scatchDepth - 1);

  if(setjmp(sc->buf))
  {
    sc = _StentCatchAtDepth(_scatchDepth - 1);
    *ex = sc->ex;
    _scatchDepth--;

    return 1;
  }

  func(ptr);

  _scatchDepth--;

  return 0;
}
