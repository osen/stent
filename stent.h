#ifndef REF_H
#define REF_H

#include <time.h>
#include <stdlib.h>

#define REF(T) \
  struct T##Ref

struct RefObject
{
  int idx;
  void *ptr;
  int unique;
  time_t time;
  void *(*get)();
  void *(*attach)();
};

extern struct RefObject _refObject;

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
    struct T *(*get)(int, void*, int, time_t); \
    struct T##Ref *(*attach)(struct T*, char*, char*, int, int); \
  }; \
 
struct RefObject *_RefAttach(void *ptr, char *type, char *file, int line, int mustExist);
struct RefObject *_RefCalloc(size_t size, char *type, char *file, int line);
void _RefFree(struct RefObject *ref);
void _RefRelease(void *ptr);

void *_RefGet(int idx, void *ptr, int unique, time_t time);

#define GET(R) \
  (((void(*)())R.get != (void(*)())_RefGet) ? NULL : R.get(R.idx, R.ptr, R.unique, R.time))

#define CALLOC(T) \
  *((struct T##Ref*)_RefCalloc(sizeof(struct T), "struct "#T, __FILE__, __LINE__))

#define FREE(R) \
  _RefFree((struct RefObject*)&R)

#define ATTACH(R, P) \
  *(void **)(&R.attach) = _RefAttach; \
  R = *R.attach(P, "?", __FILE__, __LINE__, 0)

#define ATTACH_T(R, T, P) \
  *(void **)(&R.attach) = _RefAttach; \
  R = *R.attach(P, "struct "#T, __FILE__, __LINE__, 0)

#define REATTACH(R, P) \
  *(void **)(&R.attach) = _RefAttach; \
  R = *R.attach(P, "?", __FILE__, __LINE__, 1)

#define REATTACH_T(R, T, P) \
  *(void **)(&R.attach) = _RefAttach; \
  R = *R.attach(P, "struct "#T, __FILE__, __LINE__, 1)

void RefStats();
void RefCleanup();

//
// Array support
//

size_t _AbortIfNotLess(size_t a, size_t b);

#define ARRAY(T) \
  struct _##T##ArrayRef

#define ARRAY_ALLOC(T) \
  *((struct _##T##ArrayRef*)_RefCalloc(sizeof(struct _##T##Array), "struct "#T"[]", __FILE__, __LINE__))

#define ARRAY_FREE(A) \
  do \
  { \
    if(GET(A) == NULL) break; \
    if(GET(A)->data) free(GET(A)->data); \
    FREE(A); \
  } \
  while(0)

#define ARRAY_ADD(A, E) \
  do \
  { \
    void *tmp = NULL; \
    tmp = realloc(GET(A)->data, sizeof(*GET(A)->data) * (GET(A)->size + 1)); \
    if(tmp == NULL) abort(); \
    GET(A)->data = tmp; \
    GET(A)->data[GET(A)->size] = E; \
    GET(A)->size++; \
  } \
  while(0)

#define ARRAY_SIZE(A) \
  GET(A)->size

#define ARRAY_AT(A, I) \
  GET(A)->data[_AbortIfNotLess(I, GET(A)->size)]

#endif
