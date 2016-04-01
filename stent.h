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

#endif

