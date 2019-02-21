#ifndef STENT_H
#define STENT_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ref(T) \
  struct _stent_##T##_ref

#define array(T) \
  struct _stent_##T##_array_ref

#define refdef(T) \
  ref(T); \
  _STENT_DEF(T); \
  array(T); \
  struct T##_array \
  { \
    ref(T) *data; \
    size_t size; \
    ref(T) (*_at)(array(T), size_t, int); \
  }; \
  _STENT_DEF(T##_array)

#define _STENT_DEF(T) \
  struct _stent_##T##_ref \
  { \
    int idx; \
    struct T *ptr; \
    time_t time; \
    size_t unique; \
    struct T *(*_get)(ref(T), int, int); \
    void (*_finalizer)(ref(T), void (*)(ref(T)), int); \
    void (*_strong)(ref(T), ref(Object), int); \
    ref(Object) *(*_cast)(char *, ref(Object), int); \
    ref(Exception) (*_try_s)(void (*)(ref(T)), ref(T), int); \
    char _stent_reference_object; \
  }

#define object(T) \
  ref(T); \
  struct T

#define create(T) \
  *((struct _stent_##T##_ref *)_stent_create(sizeof(struct T), \
    "struct "#T, __FILE__, __LINE__))

#define array_create(T) \
  *((struct _stent_##T##_array_ref *)_stent_array_create( \
    "struct "#T"[]", __FILE__, __LINE__))

refdef(Object);
object(Object)
{
  int dummy;
};

refdef(Exception);
object(Exception)
{
  int errorCode;
  char *message;
  char *file;
  int line;
};

ref(Object) *_stent_object_p_from_null();

#define nullref(T) \
  *((struct _stent_##T##_ref *)_stent_object_p_from_null())

#define valid(R) \
  (((void (*)())(R)._get != (void (*)())_stent_get) ? \
    0 : (R)._get((R), 0, (&R)->_stent_reference_object) != NULL)

#define try(F, R) \
  R._try_s(F, R, _stent_throw_if_not_func((void (*)())R._try_s, \
    (void (*)())_stent_try) + (&R)->_stent_reference_object)

#define throw(C, M) \
  do \
  { \
    _stent_throw(C, M, __FILE__, __LINE__); \
    abort(); \
  } \
  while(0)

#define get(R) \
  R._get(R, 1, _stent_throw_if_not_func((void (*)())R._get, \
    (void (*)())_stent_get) + (&R)->_stent_reference_object)

#define destroy(R) \
    _stent_destroy(*((ref(Object) *)&R), (&R)->_stent_reference_object);

#define strong(P, C) \
    P._strong(P, *((ref(Object) *)&C), \
      (&P)->_stent_reference_object + (&C)->_stent_reference_object);

#define finalizer(R, F) \
  R._finalizer(R, F, _stent_throw_if_not_func((void (*)())R._finalizer, \
    (void (*)())_stent_finalizer) + (&R)->_stent_reference_object)

#define cast(T, R) \
  *((struct _stent_##T##_ref *)R._cast("struct "#T, *((ref(Object) *)&R), (&R)->_stent_reference_object))

#define array_size(A) \
  get(A)->size

#define array_at(A, I) \
  get(A)->_at(A, I, _stent_throw_if_not_func((void (*)())get(A)->_at, \
    (void (*)())_stent_array_at) + (&A)->_stent_reference_object)

#define array_add(A, R) \
  do \
  { \
    void *tmp; \
    tmp = realloc(get(A)->data, sizeof(*get(A)->data) * (get(A)->size + 1)); \
    if(!tmp) throw(0, "Failed to increase size of array"); \
    get(A)->data = tmp; \
    get(A)->data[get(A)->size] = R; \
    get(A)->size++; \
  } \
  while(0)

#define array_remove(A, I) \
  do \
  { \
    void *tmp; \
    memmove(get(A)->data + I, get(A)->data + I + 1, sizeof(*get(A)->data) * (get(A)->size - 1)); \
    tmp = realloc(get(A)->data, sizeof(*get(A)->data) * (get(A)->size - 1)); \
    if(!tmp) throw(0, "Failed to decrease size of array"); \
    get(A)->data = tmp; \
    get(A)->size--; \
  } \
  while(0)

void stent_cleanup();

ref(Object) *_stent_create(size_t size, char *type, char *file, int line);
ref(Object) *_stent_array_create(char *type, char *file, int line);
ref(Object) _stent_array_at(array(Object), size_t, int unused);
int _stent_throw_if_not_func(void (*a)(), void (*b)());
void *_stent_get(ref(Object) obj, int throws, int unused);
void _stent_destroy(ref(Object) obj, int unused);
void _stent_strong(ref(Object) parent, ref(Object) child, int unused);
void _stent_finalizer(ref(Object) obj, void (*func)(ref(Object)), int unused);
ref(Object) *_stent_cast(char * type, ref(Object) obj, int unused);
void _stent_throw(int code, const char *message, char *file, int line);

ref(Exception) _stent_try(void (*func)(ref(Object)), ref(Object) userData,
  int unused);

/*
 * Exception accessor functions
 */

char *ExceptionMessage(ref(Exception) ctx);

/*
 * Primitive array support
 */

#define _stent_ARRAY_DEC(T, N) \
  refdef(N##Array); \
  ref(N##Array) N##ArrayCreate(); \
  void N##ArrayResize(ref(N##Array), size_t); \
  T* N##ArrayGet(ref(N##Array)); \
  T N##ArrayAt(ref(N##Array), size_t); \
  size_t N##ArraySize(ref(N##Array)); \
  void N##ArrayAdd(ref(N##Array), T); \
  void N##ArraySet(ref(N##Array), size_t, T);

_stent_ARRAY_DEC(char, Char)
_stent_ARRAY_DEC(size_t, Size_t)

refdef(String);

ref(String) StringCreate();
ref(String) StringFromCStr(const char *str);
ref(String) StringFromSubstring(ref(String), size_t, size_t);
char *StringCStr(ref(String) ctx);
void StringAddCStr(ref(String) ctx, char *str);
void StringSetCStr(ref(String) ctx, char *str);
void StringAddInt(ref(String) ctx, int val);
int StringCompareCStr(ref(String) ctx, char *str);
void StringAddChar(ref(String) ctx, char c);
void StringAddCharArray(ref(String) ctx, ref(CharArray) toadd, size_t start, size_t len);
char StringAt(ref(String), size_t);
size_t StringLength(ref(String));
ref(CharArray) StringCharArray(ref(String));

#endif
