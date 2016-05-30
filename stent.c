#include "stent.h"

#include <stdio.h>
#include <string.h>

struct RefData
{
  void *ptr;
  int unique;
  time_t time;
  char *type;
  char *file;
  int line;
  void (*finalizer)(REF(Object));
};

REF(Object) _refObject;

struct RefData **_refs;
size_t _refCount;

time_t _refTime;
int _refUnique;

void _RefFree(REF(Object) *ref)
{
  struct RefData *refData = NULL;
  REF(Object) _ref = *ref;

  if(TRYGET(_ref) == NULL)
  {
    return;
  }

  refData = _refs[ref->idx];

  if(refData->finalizer != NULL)
  {
    refData->finalizer(_ref);
  }

  free(refData->ptr);
  refData->ptr = NULL;
}

void RefStats()
{
  int i = 0;
  struct RefData *refData = NULL;

  for(i = 0; i < _refCount; i++)
  {
    refData = _refs[i];

    if(refData == NULL)
    {
      break;
    }

    if(refData->ptr == NULL)
    {
      continue;
    }

    printf("\n");
    printf("***** Memory Leak ******\n");
    printf("Type: %s\n", refData->type);
    printf("File: %s\n", refData->file);
    printf("Line: %i\n", refData->line);
  }
}

void RefCleanup()
{
  int i = 0;
  struct RefData *refData = NULL;

  for(i = 0; i < _refCount; i++)
  {
    refData = _refs[i];

    if(refData == NULL)
    {
      break;
    }

    free(refData->type);
    free(refData->file);
    free(refData);
    _refs[i] = NULL;
  }

  free(_refs);
  _refs = NULL;
  _refCount = 0;
}

REF(Object) *_RefCalloc(size_t size, char *type, char *file, int line)
{
  size_t i = 0;
  struct RefData *refData = NULL;
  time_t currentTime = 0;
  void *ptr = NULL;

  ptr = calloc(1, size);

  if(ptr == NULL)
  {
    memset(&_refObject, 0, sizeof(_refObject));

    return &_refObject;
  }

  if(_refs == NULL)
  {
    _refs = calloc(32, sizeof(*_refs));
    _refCount = 32;
  }

  for(i = 0; i < _refCount; i++)
  {
    if(_refs[i] == NULL)
    {
      refData = calloc(1, sizeof(struct RefData));
      _refs[i] = refData;
      break;
    }

    if(_refs[i]->ptr == NULL)
    {
      refData = _refs[i];
      if(refData->file) free(refData->file);
      if(refData->type) free(refData->type);
      break;
    }
  }

  if(refData == NULL)
  {
    _refs = realloc(_refs, (_refCount * 2) * sizeof(*_refs));
    memset(_refs + _refCount, 0, _refCount * sizeof(*_refs));
    i = _refCount;
    _refCount *= 2;
    refData = calloc(1, sizeof(struct RefData));
    _refs[i] = refData;
  }

  time(&currentTime);

  if(currentTime != _refTime)
  {
    _refTime = currentTime;
    _refUnique = 0;
  }

  refData->ptr = ptr;
  refData->unique = _refUnique;
  refData->time = _refTime;
  refData->type = strdup(type);
  refData->file = strdup(file);
  refData->line = line;
  refData->finalizer = NULL;

  _refUnique++;

  _refObject.idx = i;
  *(void **)(&_refObject.cast) = _RefCast;
  *(void **)(&_refObject.get) = _RefGet;
  *(void **)(&_refObject.finalizer) = _RefFinalizer;
  _refObject.ptr = refData->ptr;
  _refObject.unique = refData->unique;
  _refObject.time = refData->time;

  return &_refObject;
}

void _RefFinalizer(REF(Object) obj, void (*finalizer)(REF(Object)))
{
  struct RefData *refData = NULL;

  if(GET(obj) == NULL)
  {
    return;
  }

  refData = _refs[obj.idx];
  refData->finalizer = finalizer;
}

void *_RefCast(REF(Object) obj, char *type)
{
  struct RefData *refData = NULL;

  if(GET(obj) == NULL)
  {
    THROW(0, "Reference is no longer valid");
  }

  refData = _refs[obj.idx];
  _refObject = obj;

  if(strcmp(type, "struct Object") == 0)
  {
    return &_refObject;
  }

  if(strcmp(type, refData->type) != 0)
  {
    THROW(0, "Invalid cast type");
  }

  return &_refObject;
}

void *_RefGet(REF(Object) obj, int throws)
{
  struct RefData *refData = NULL;

  if(obj.idx >= _refCount)
  {
    if(throws == 1) goto invalid;
    return NULL;
  }

  refData = _refs[obj.idx];

  if(refData->ptr != obj.ptr)
  {
    if(throws == 1) goto invalid;
    return NULL;
  }

  if(refData->unique != obj.unique)
  {
    if(throws == 1) goto invalid;
    return NULL;
  }

  if(refData->time != obj.time)
  {
    if(throws == 1) goto invalid;
    return NULL;
  }

  return refData->ptr;

invalid:
  THROW(0, "Reference is no longer valid");
}

void ArrayFinalizer(ARRAY(Object) obj)
{
  if(GET(obj)->data) free(GET(obj)->data);
}

REF(Object) *_AddArrayFinalizer(REF(Object) *ctx)
{
  ARRAY(Object) arr = *((ARRAY(Object)*)ctx);

  FINALIZER(arr, ArrayFinalizer);

  return ctx;
}

size_t _AbortIfNotLess(size_t a, size_t b)
{
  if(a >= b)
  {
    THROW(0, "Attempted out of bounds array access");
  }

  return a;
}
