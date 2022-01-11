#ifndef _VEC_H
#define _VEC_H

#include <stdlib.h>

#define vec(T) \
  T **

#define vec_new(T) \
  (vec(T))_vec_new(sizeof(T))

#define vec_delete(V) \
  _vec_delete((vec(void))V)

#define vec_size(V) \
  _vec_size((vec(void))V)

#define vec_compare(V1, V2) \
  _vec_compare((vec(void))V1, (vec(void))V2)

#define vec_push_back(V, E) \
  do \
  { \
    _vec_resize((vec(void))V, vec_size(V) + 1); \
    V[0][vec_size(V) - 1] = E; \
  } \
  while(0)

#define vec_resize(V, S) \
  _vec_resize((vec(void))V, S)

#define vec_clear(V) \
  _vec_clear((vec(void))V)

#define vec_at(V, I) \
   (V[0][I])

#define vec_erase(V, I, N) \
  _vec_erase((vec(void))V, I, N)

#define vec_insert(V, B, S, I, N) \
  _vec_insert((vec(void))V, B, (vec(void))S, I, N)

#define vec_push(V, E) \
  vec_push_back(V, E)

#define vec_fill(V, S, N, D) \
  do \
  { \
    size_t __start = S; \
    size_t __num = N; \
    size_t vi = 0; \
    vec_at(V, __start) = D; \
    for(vi = __start; vi < __start + __num; ++vi) \
    { \
      vec_at(V, vi) = vec_at(V, __start); \
    } \
  } \
  while(0)

#define foreach(VAR, VEC, BODY) \
  { \
    size_t __var_i = 0; \
    size_t __var_size = vec_size(VEC); \
    for(; __var_i < __var_size; ++__var_i) \
    { \
      VAR = vec_at(VEC, __var_i); \
      BODY \
    } \
  }

#ifdef __cplusplus
extern "C"
{
#endif

vec(void) _vec_new(size_t size);
void _vec_delete(vec(void) ptr);
size_t _vec_size(vec(void) ptr);
int _vec_compare(vec(void) ptr1, vec(void) ptr2);
void _vec_resize(vec(void) ptr, size_t size);
void _vec_clear(vec(void) ptr);
void _vec_erase(vec(void) ptr, size_t idx, size_t num);

void _vec_insert(vec(void) ptr, size_t before,
  vec(void) source, size_t idx, size_t num);

void panic(const char *message);

#ifdef __cplusplus
}
#endif

#ifdef VEC_IMPLEMENTATION
#undef VEC_IMPLEMENTATION

#include <stdio.h>
#include <string.h>

/***************************************************
 * Vector
 ***************************************************/

struct Vec
{
  char *data;
  size_t size;
  size_t allocated;
  size_t elementSize;
};

#ifdef __cplusplus
extern "C"
{
#endif

vec(void) _vec_new(size_t size)
{
  struct Vec *rtn = calloc(1, sizeof(*rtn));
  if(!rtn) return NULL;

  rtn->elementSize = size;

  return (vec(void))rtn;
}

void _vec_delete(vec(void) ptr)
{
  struct Vec *v = (struct Vec *)ptr;

  free(v->data);
  free(v);
}

size_t _vec_size(vec(void) ptr)
{
  struct Vec *v = (struct Vec *)ptr;

  return v->size;
}

int _vec_compare(vec(void) ptr1, vec(void) ptr2)
{
  struct Vec *v1 = (struct Vec *)ptr1;
  struct Vec *v2 = (struct Vec *)ptr2;

  /*
   * TODO: Check if 1 or -1.
   */
  if(v1->size != v2->size)
  {
    return 1;
  }

  return memcmp(v1->data, v2->data, v1->size);
}

void _vec_clear(vec(void) ptr)
{
  struct Vec *v = (struct Vec *)ptr;

  v->size = 0;
}

void _vec_resize(vec(void) ptr, size_t size)
{
  struct Vec *v = (struct Vec *)ptr;
  size_t s = 0;
  void *d = NULL;

  if(v->allocated >= size)
  {
    v->size = size;
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

  d = calloc(s, v->elementSize);

  if(!d)
  {
    fprintf(stderr, "Error: Failed to increase vec size\n");
    abort();
  }

  memcpy(d, v->data, v->elementSize * v->size);
  free(v->data);
  v->data = d;
  v->allocated = s;
  v->size = size;
}

size_t _vec_valid(vec(void) ptr, size_t idx)
{
  struct Vec *v = (struct Vec *)ptr;

  if(v->size > idx)
  {
    return idx;
  }

  fprintf(stderr, "Error: Index [index=%lu] out of bounds [size=%lu]\n",
    (unsigned long)idx, (unsigned long)v->size);
  abort();
}

void _vec_erase(vec(void) ptr, size_t idx, size_t num)
{
  struct Vec *v = (struct Vec *)ptr;
  char *dest = NULL;
  char *src = NULL;
  size_t tm = 0;

  if(idx >= v->size ||
    idx + num > v->size)
  {
    fprintf(stderr, "Error: Index out of bounds [size=%i] [index=%i] [num=%i]\n", (int)v->size, (int)idx, (int)num);
    abort();
  }

  if(!num)
  {
    return;
  }

  dest = (char *)v->data;
  dest += idx * v->elementSize;

  src = dest;
  src += num * v->elementSize;

  tm = (v->size - (idx + num)) * v->elementSize;

  if(tm)
  {
    memmove(dest, src, tm);
  }

  v->size -= num;
}

void _vec_insert(vec(void) ptr, size_t before,
  vec(void) source, size_t idx, size_t num)
{
  struct Vec *d = (struct Vec *)ptr;
  struct Vec *s = (struct Vec *)source;
  char *dest = NULL;
  char *src = NULL;
  size_t tm = 0;

  if(s == d)
  {
    fprintf(stderr, "Error: Source and desination must not match\n");
    abort();
  }

  if(!num)
  {
    return;
  }

  if(before > d->size)
  {
    fprintf(stderr, "Error: Invalid index specified. Non contiguous\n");
    abort();
  }

  if(idx >= s->size ||
    idx + num > s->size)
  {
    fprintf(stderr, "Error: Index out of bounds on source\n");
    abort();
  }

  tm = (d->size - before) * d->elementSize;

  _vec_resize(ptr, d->size + num);

  src = (char *)d->data;
  src += (before * d->elementSize);
  dest = src;
  dest += (num * d->elementSize);
  memmove(dest, src, tm);

  dest = src;
  src = (char *)s->data;
  src += (idx * d->elementSize);
  memcpy(dest, src, num * s->elementSize);
}

void panic(const char *message)
{
  printf("Panic: %s\n", message);

  abort();
}

#ifdef __cplusplus
}
#endif

#endif

#endif
