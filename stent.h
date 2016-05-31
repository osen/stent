#ifndef REF_H
#define REF_H

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define REF(T) \
  struct T##Ref

#define REFDEF(T) \
  struct T; \
  DECLARE(T); \
  struct _##T##Array \
  { \
    REF(T) *data; \
    int size; \
  }; \
  DECLARE(_##T##Array)

#define DECLARE(T) \
  struct T##Ref \
  { \
    int idx; \
    struct T *ptr; \
    int unique; \
    time_t time; \
    struct T *(*get)(REF(T), int, int unused); \
    void (*finalizer)(REF(T), void (*)(REF(T))); \
    void *(*cast)(REF(T) obj, char *type, int unused); \
    REF(Exception) (*try)(void (*func)(REF(T)), REF(T) userData, int unused); \
  };

struct Object
{
  int dummy;
};

REFDEF(Object);

struct Exception
{
  int errorCode;
  char *message;
  char *file;
  int line;
};

REFDEF(Exception);

//
// Exception support
//

#define THROW(C, M) \
  do \
  { \
    _RefThrow(C, M, __FILE__, __LINE__); \
    abort(); \
  } \
  while(0)
 
REF(Object) *_RefCalloc(size_t size, char *type, char *file, int line);
void _RefFree(REF(Object) *ref);
void _RefThrow(int code, char *message, char *file, int line);

void *_RefGet(REF(Object) obj, int throws, int unused);
void _RefFinalizer(REF(Object) obj, void (*finalizer)(REF(Object)));
void *_RefCast(REF(Object) obj, char *type, int unused);
REF(Exception) _RefTry(void (*func)(REF(Object)), REF(Object) userData, int unused);

int _RefThrowIfNotFunc(void(*a)(), void(*b)());

#define TRY(F, R) \
  R.try(F, R, _RefThrowIfNotFunc((void(*)())R.try, (void(*)())_RefTry))

#define CAST(T, R) \
  *((struct T##Ref*)R.cast(R, "struct "#T, _RefThrowIfNotFunc((void(*)())R.cast, (void(*)())_RefCast)))

#define GET(R) \
  R.get(R, 1, _RefThrowIfNotFunc((void(*)())R.get, (void(*)())_RefGet))

#define TRYGET(R) \
  (((void(*)())R.get != (void(*)())_RefGet) ? NULL : R.get(R, 0, 0))

#define CALLOC(T) \
  *((struct T##Ref*)_RefCalloc(sizeof(struct T), "struct "#T, __FILE__, __LINE__))

#define FINALIZER(R, F) \
  do \
  { \
    if((void(*)())R.finalizer != (void(*)())_RefFinalizer) \
    { \
      THROW(0, "Attempt to set finalizer on NULL pointer"); \
    } \
    R.finalizer(R, F); \
  } \
  while(0)

#define FREE(R) \
  do \
  { \
    if((void(*)())R.get != (void(*)())_RefGet) \
    { \
      THROW(0, "Attempt to free uninitialized reference"); \
    } \
    _RefFree((REF(Object)*)&R); \
  } \
  while(0)

void RefStats();
void RefCleanup();

//
// Array support
//

REF(Object) *_AddArrayFinalizer(REF(Object) *ctx);
size_t _AbortIfNotLess(size_t a, size_t b);

#define ARRAY(T) \
  struct _##T##ArrayRef

#define ARRAY_ALLOC(T) \
  *((struct _##T##ArrayRef*)_AddArrayFinalizer(_RefCalloc(sizeof(struct _##T##Array), "struct "#T"[]", __FILE__, __LINE__)))

#define ARRAY_ADD(A, E) \
  do \
  { \
    void *tmp = NULL; \
    tmp = realloc(GET(A)->data, sizeof(*GET(A)->data) * (GET(A)->size + 1)); \
    if(tmp == NULL) THROW(0, "Failed to increase size of array"); \
    GET(A)->data = tmp; \
    GET(A)->data[GET(A)->size] = E; \
    GET(A)->size++; \
  } \
  while(0)

#define ARRAY_SIZE(A) \
  GET(A)->size

#define ARRAY_AT(A, I) \
  GET(A)->data[_AbortIfNotLess(I, GET(A)->size)]

#define ARRAY_REMOVEAT(A, I) \
  do \
  { \
    int ri = 0; \
    ri = I; \
    _AbortIfNotLess(ri, GET(A)->size); \
    if(ri != GET(A)->size - 1) \
    { \
      memmove(GET(A)->data + ri, GET(A)->data + (ri + 1), sizeof(*GET(A)->data) * (GET(A)->size - (ri + 1))); \
    } \
    GET(A)->size--; \
  } \
  while(0)

#endif
