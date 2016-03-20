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
};

#define DECLARE(T) \
  struct T; \
  struct T##Ref \
  { \
    int idx; \
    struct T *ptr; \
    int unique; \
    time_t time; \
    struct T *(*get)(int idx, void *ptr, int unique, time_t time); \
  }; \
  inline static struct T##Ref T##RefCalloc(size_t size, char *type, char *file, int line) \
  { \
    struct T##Ref rtn = {}; \
    _RefCalloc((struct RefObject*)&rtn, size, type, file, line); \
    return rtn; \
  }
 
void _RefCalloc(struct RefObject *ref, size_t size, char *type, char *file, int line);
void _RefFree(struct RefObject *ref);

void *_RefGet(int idx, void *ptr, int unique, time_t time);

#define GET(R) \
  (((void(*)())R.get != (void(*)())_RefGet) ? NULL : R.get(R.idx, R.ptr, R.unique, R.time))

#define CALLOC(T) \
  T##RefCalloc(sizeof(struct T), "struct "#T, __FILE__, __LINE__)

#define FREE(R) \
  _RefFree((struct RefObject*)&R)

void RefStats();
void RefCleanup();

#endif

