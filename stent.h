#ifndef REF_H
#define REF_H

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define REF(T) \
  struct T##Ref

#define REFDEF(T)      \
  struct T;            \
  DECLARE(T);          \
  struct _##T##Array   \
  {                    \
    REF(T) *data;      \
    int size;          \
  };                   \
  DECLARE(_##T##Array)

#define DECLARE(T)                                                            \
  struct T##Ref                                                               \
  {                                                                           \
    int idx;                                                                  \
    struct T *ptr;                                                            \
    int unique;                                                               \
    time_t time;                                                              \
    struct T *(*get)(REF(T), int, int unused);                                \
    void (*finalizer)(REF(T), void (*)(REF(T)));                              \
    void *(*cast)(REF(T) obj, char *type, int unused);                        \
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

int _StentThrowIfNotFunc(void(*a)(), void(*b)());
size_t _StentThrowIfNotLess(size_t a, size_t b);

void _StentThrow(int code, char *message, char *file, int line);
#define THROW(C, M)                        \
  do                                       \
  {                                        \
    _StentThrow(C, M, __FILE__, __LINE__); \
    abort();                               \
  }                                        \
  while(0)

REF(Exception) _StentTry(void (*func)(REF(Object)),
  REF(Object) userData, int unused);

#define TRY(F, R)                                    \
  R.try(F, R, _StentThrowIfNotFunc((void(*)())R.try, \
    (void(*)())_StentTry))

void *_StentCast(REF(Object) obj, char *type, int unused);

#define CAST(T, R)                          \
  *((struct T##Ref*)R.cast(R, "struct "#T,  \
    _StentThrowIfNotFunc((void(*)())R.cast, \
    (void(*)())_StentCast)))

void *_StentGet(REF(Object) obj, int throws, int unused);

#define TRYGET(R)                               \
  (((void(*)())R.get != (void(*)())_StentGet) ? \
    NULL : R.get(R, 0, 0))

#define GET(R)                                       \
  R.get(R, 1, _StentThrowIfNotFunc((void(*)())R.get, \
    (void(*)())_StentGet))

REF(Object) *_StentCalloc(size_t size, char *type, char *file, int line);

#define CALLOC(T)                                  \
  *((struct T##Ref*)_StentCalloc(sizeof(struct T), \
    "struct "#T, __FILE__, __LINE__))

void _StentFinalizer(REF(Object) obj, void (*finalizer)(REF(Object)));

#define FINALIZER(R, F)                                      \
  do                                                         \
  {                                                          \
    if((void(*)())R.finalizer != (void(*)())_StentFinalizer) \
    {                                                        \
      THROW(0, "Attempt to set finalizer on NULL pointer");  \
    }                                                        \
    R.finalizer(R, F);                                       \
  }                                                          \
  while(0)

void _StentFree(REF(Object) *ref);

#define FREE(R)                                            \
  do                                                       \
  {                                                        \
    if((void(*)())R.get != (void(*)())_StentGet)           \
    {                                                      \
      THROW(0, "Attempt to free uninitialized reference"); \
    }                                                      \
    _StentFree((REF(Object)*)&R);                          \
  }                                                        \
  while(0)

void StentStats();
void StentCleanup();

//
// Array support
//

REF(Object) *_StentAddArrayFinalizer(REF(Object) *ctx);

#define ARRAY(T) \
  struct _##T##ArrayRef

#define ARRAY_ALLOC(T)                          \
  *((struct _##T##ArrayRef*)_StentAddArrayFinalizer( \
    _StentCalloc(sizeof(struct _##T##Array),    \
    "struct "#T"[]", __FILE__, __LINE__)))

#define ARRAY_ADD(A, E)                                                      \
  do                                                                         \
  {                                                                          \
    void *tmp = NULL;                                                        \
    tmp = realloc(GET(A)->data, sizeof(*GET(A)->data) * (GET(A)->size + 1)); \
    if(tmp == NULL) THROW(0, "Failed to increase size of array");            \
    GET(A)->data = tmp;                                                      \
    GET(A)->data[GET(A)->size] = E;                                          \
    GET(A)->size++;                                                          \
  }                                                                          \
  while(0)

#define ARRAY_SIZE(A) \
  GET(A)->size

#define ARRAY_AT(A, I) \
  GET(A)->data[_StentThrowIfNotLess(I, GET(A)->size)]

#define ARRAY_CLEAR(A)     \
  while(ARRAY_SIZE(A) > 0) \
  {                        \
    ARRAY_REMOVEAT(A, 0);  \
  }

#define ARRAY_REMOVEAT(A, I)                                \
  do                                                        \
  {                                                         \
    int ri = 0;                                             \
    ri = I;                                                 \
    _StentThrowIfNotLess(ri, GET(A)->size);                 \
    if(ri != GET(A)->size - 1)                              \
    {                                                       \
      memmove(GET(A)->data + ri, GET(A)->data + (ri + 1),   \
        sizeof(*GET(A)->data) * (GET(A)->size - (ri + 1))); \
    }                                                       \
    GET(A)->size--;                                         \
  }                                                         \
  while(0)

#endif

