#include "stent.h"

#include <stdio.h>
#include <string.h>

#define MAX_REFS 2048

struct RefData
{
  void *ptr;
  int unique;
  time_t time;
  char *type;
  char *file;
  int line;
};

struct RefObject _refObject;
struct RefData *_refs[MAX_REFS];
time_t _refTime;
int _refUnique;

void _RefRelease(void *ptr)
{
  int i = 0;
  struct RefData *refData = NULL;

  for(i = 0; i < MAX_REFS; i++)
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

    if(refData->ptr == ptr)
    {
      refData->ptr = NULL;
      return;
    }
  }
}

void _RefFree(struct RefObject *ref)
{
  struct RefData *refData = NULL;
  struct RefObject _ref = *ref;

  if(GET(_ref) == NULL)
  {
    return;
  }

  refData = _refs[ref->idx];

  free(refData->ptr);
  refData->ptr = NULL;
}

void RefStats()
{
  int i = 0;
  struct RefData *refData = NULL;

  for(i = 0; i < MAX_REFS; i++)
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

  for(i = 0; i < MAX_REFS; i++)
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
}

struct RefObject *_RefPlace(void *ptr, char *type, char *file, int line)
{
  int i = 0;
  struct RefData *refData = NULL;
  time_t currentTime = 0;

  if(ptr == NULL)
  {
    memset(&_refObject, 0, sizeof(_refObject));

    return &_refObject;
  }

  for(i = 0; i < MAX_REFS; i++)
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
    printf("Error: Max references reached\n");
    abort();
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

  _refUnique++;

  _refObject.idx = i;
  _refObject.get = _RefGet;
  _refObject.ptr = refData->ptr;
  _refObject.unique = refData->unique;
  _refObject.time = refData->time;

  return &_refObject;
}

struct RefObject *_RefAttach(void *ptr, char *type, char *file, int line, int mustExist)
{
  int i = 0;
  struct RefData *refData = NULL;
  int checkType = 0;

  if(strcmp(type, "?") != 0)
  {
    checkType = 1;
  }

  for(i = 0; i < MAX_REFS; i++)
  {
    if(_refs[i] == NULL)
    {
      break;
    }

    if(_refs[i]->ptr == ptr)
    {
      refData = _refs[i];

      if(checkType == 1)
      {
        if(strcmp(refData->type, type) != 0)
        {
          break;
        }
      }

      _refObject.idx = i;
      _refObject.get = _RefGet;
      _refObject.ptr = refData->ptr;
      _refObject.unique = refData->unique;
      _refObject.time = refData->time;

      return &_refObject;
    }
  }

  if(mustExist == 1)
  {
    memset(&_refObject, 0, sizeof(_refObject));

    return &_refObject;
  }

  return _RefPlace(ptr, type, file, line);
}

struct RefObject *_RefCalloc(size_t size, char *type, char *file, int line)
{
  return _RefPlace(calloc(1, size), type, file, line);
}

void *_RefGet(int idx, void *ptr, int unique, time_t time)
{
  struct RefData *refData = NULL;

  refData = _refs[idx];

  if(refData->ptr != ptr)
  {
    return NULL;
  }

  if(refData->unique != unique)
  {
    return NULL;
  }

  if(refData->time != time)
  {
    return NULL;
  }

  return refData->ptr;
}

size_t _AbortIfNotLess(size_t a, size_t b)
{
  if(a < b)
  {
    return a;
  }

  abort();
}
