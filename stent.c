#include "stent.h"

#include <stdio.h>
#include <string.h>
#include <setjmp.h>

struct RefData
{
  void *ptr;
  int unique;
  time_t time;
  char *type;
  char *file;
  int line;
  void (*finalizer)(REF(Object));
  int exceptionLevel;
};

struct ExceptionData
{
  jmp_buf buf;
  int code;
  char *message;
  char *file;
  int line;
};

static struct
{
  REF(Object) tempObject;
  time_t currentTime;
  int unique;
  struct RefData **refs;
  size_t refCount;
  size_t exceptionStackCount;
  size_t exceptionStackLocation;
  struct ExceptionData *exceptionStack;
} stent;

int _RefThrowIfNotFunc(void(*a)(), void(*b)())
{
  if(a != b)
  {
    THROW(0, "Reference has not yet been initialized");
  }

  return 0;
}

void _RefThrow(int code, char *message, char *file, int line)
{
  struct ExceptionData *exceptionData = NULL;

  if(stent.exceptionStackLocation <= 0)
  {
    printf("Unhandled Exception (%i): %s\nFILE: %s\nLINE: %i\n",
      code, message, file, line);

    abort();
  }

  exceptionData = &stent.exceptionStack[stent.exceptionStackLocation - 1];

  exceptionData->code = code;
  exceptionData->message = message;
  exceptionData->file = file;
  exceptionData->line = line;
  longjmp(exceptionData->buf, 1);
}

void _RefFree(REF(Object) *ref)
{
  REF(Object) _ref = *ref;
  struct RefData *refData = NULL;

  if(TRYGET(_ref) == NULL)
  {
    return;
  }

  refData = stent.refs[_ref.idx];

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

  for(i = 0; i < stent.refCount; i++)
  {
    refData = stent.refs[i];

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

  for(i = 0; i < stent.refCount; i++)
  {
    refData = stent.refs[i];

    if(refData == NULL)
    {
      break;
    }

    free(refData->type);
    free(refData->file);
    free(refData);
  }

  free(stent.refs);
  free(stent.exceptionStack);
  memset(&stent, 0, sizeof(stent));
}

REF(Object) *_RefFromRefData(int idx)
{
  memset(&stent.tempObject, 0, sizeof(stent.tempObject));

  if(idx >= stent.refCount)
  {
    return &stent.tempObject;
  }

  if(stent.refs[idx] == NULL)
  {
    return &stent.tempObject;
  }

  stent.tempObject.idx = idx;
  *(void **)(&stent.tempObject.cast) = _RefCast;
  *(void **)(&stent.tempObject.get) = _RefGet;
  *(void **)(&stent.tempObject.finalizer) = _RefFinalizer;
  *(void **)(&stent.tempObject.try) = _RefTry;
  stent.tempObject.ptr = stent.refs[idx]->ptr;
  stent.tempObject.unique = stent.refs[idx]->unique;
  stent.tempObject.time = stent.refs[idx]->time;

  return &stent.tempObject;
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
    memset(&stent.tempObject, 0, sizeof(stent.tempObject));

    return &stent.tempObject;
  }

  if(stent.refs == NULL)
  {
    stent.refs = calloc(32, sizeof(*stent.refs));
    stent.refCount = 32;
  }

  for(i = 0; i < stent.refCount; i++)
  {
    if(stent.refs[i] == NULL)
    {
      refData = calloc(1, sizeof(struct RefData));
      stent.refs[i] = refData;
      break;
    }

    if(stent.refs[i]->ptr == NULL)
    {
      refData = stent.refs[i];
      if(refData->file) free(refData->file);
      if(refData->type) free(refData->type);
      break;
    }
  }

  if(refData == NULL)
  {
    stent.refs = realloc(stent.refs,
      (stent.refCount * 2) * sizeof(*stent.refs));

    memset(stent.refs + stent.refCount, 0,
      stent.refCount * sizeof(*stent.refs));

    i = stent.refCount;
    stent.refCount *= 2;
    refData = calloc(1, sizeof(struct RefData));
    stent.refs[i] = refData;
  }

  time(&currentTime);

  if(currentTime != stent.currentTime)
  {
    stent.currentTime = currentTime;
    stent.unique = 0;
  }

  refData->ptr = ptr;
  refData->unique = stent.unique;
  refData->time = stent.currentTime;
  refData->type = strdup(type);
  refData->file = strdup(file);
  refData->line = line;
  refData->finalizer = NULL;
  refData->exceptionLevel = stent.exceptionStackLocation;

  stent.unique++;

  return _RefFromRefData(i);
}

void _RefExceptionFinalizer(REF(Exception) ctx)
{
  if(GET(ctx)->message)
  {
    free(GET(ctx)->message);
  }

  if(GET(ctx)->file)
  {
    free(GET(ctx)->file);
  }
}

void _RefReleaseExceptionLevel(int exceptionLevel, int performFree)
{
  REF(Object) current = {};
  size_t i = 0;

  for(i = 0; i < stent.refCount; i++)
  {
    current = *_RefFromRefData(i);

    if(TRYGET(current) == NULL)
    {
      continue;
    }

    if(stent.refs[i]->exceptionLevel == exceptionLevel)
    {
      if(performFree)
      {
        FREE(current);
      }
      else
      {
        stent.refs[i]->exceptionLevel--;
      }
    }
  }
}

REF(Exception) _RefTry(void (*func)(REF(Object)), REF(Object) userData,
  int unused)
{
  REF(Exception) rtn = {};
  struct ExceptionData *exceptionData = NULL;

  stent.exceptionStackLocation++;

  if(stent.exceptionStackLocation >= stent.exceptionStackCount)
  {
    stent.exceptionStack = realloc(stent.exceptionStack,
      stent.exceptionStackLocation * sizeof(*stent.exceptionStack));
 
    stent.exceptionStackCount = stent.exceptionStackLocation;
  }

  exceptionData = &stent.exceptionStack[stent.exceptionStackLocation - 1];

  if(setjmp(exceptionData->buf) == 0)
  {
    func(userData);
    _RefReleaseExceptionLevel(stent.exceptionStackLocation, 0);
    stent.exceptionStackLocation--;
  }
  else
  {
    _RefReleaseExceptionLevel(stent.exceptionStackLocation, 1);
    stent.exceptionStackLocation--;
    rtn = CALLOC(Exception);
    FINALIZER(rtn, _RefExceptionFinalizer);
    GET(rtn)->message = strdup(exceptionData->message);
    GET(rtn)->errorCode = exceptionData->code;
    GET(rtn)->file = strdup(exceptionData->file);
    GET(rtn)->line = exceptionData->line;
  }

  return rtn;
}

void _RefFinalizer(REF(Object) obj, void (*finalizer)(REF(Object)))
{
  struct RefData *refData = NULL;

  if(TRYGET(obj) == NULL)
  {
    THROW(0, "Failed to add finalizer because reference is no longer valid");
  }

  refData = stent.refs[obj.idx];
  refData->finalizer = finalizer;
}

void *_RefCast(REF(Object) obj, char *type, int unused)
{
  struct RefData *refData = NULL;

  if(GET(obj) == NULL)
  {
    THROW(0, "Failed to cast because reference is no longer valid");
  }

  refData = stent.refs[obj.idx];
  stent.tempObject = obj;

  if(strcmp(type, "struct Object") == 0)
  {
    return &stent.tempObject;
  }

  if(strcmp(type, refData->type) != 0)
  {
    THROW(0, "Invalid cast type");
  }

  return &stent.tempObject;
}

void *_RefGet(REF(Object) obj, int throws, int unused)
{
  struct RefData *refData = NULL;

  if(obj.idx >= stent.refCount)
  {
    if(throws == 1) goto invalid;
    return NULL;
  }

  refData = stent.refs[obj.idx];

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

size_t _RefThrowIfNotLess(size_t a, size_t b)
{
  if(a >= b)
  {
    THROW(0, "Attempted out of bounds array access");
  }

  return a;
}

