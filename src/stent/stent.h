#ifndef STENT_STENT_H
#define STENT_STENT_H

#include <stdlib.h>
#include <string.h>

/*
#define STENT_ENABLE
*/
#define STENT_ENABLE

#define STENT_BLOCKSIZE 1024
#define STENT_EXCEPTION_MESSAGE_LENGTH 1024

#ifdef STENT_ENABLE

/***************************************************
 * Reference
 ***************************************************/

#define ref(T) \
  T **

#define salloc(T) \
  (ref(T))_salloc(sizeof(T), #T, NULL)

#define salloc_placement(T, F) \
  (1 || (((ref(T))NULL)[0] = F) ? (ref(T))_salloc(0, #T, F) : NULL)

#define sfree(R) \
  do \
  { \
    memcmp(&R, &R, 0); \
    memcmp(R[0], R[0], 0); \
    _sfree((ref(void))R, __FILE__, __LINE__); \
  } \
  while(0)

#define _(R) \
  (_svalid((ref(void))R, __FILE__, __LINE__) || \
    memcmp(&R, &R, 0) || \
    memcmp(R[0], R[0], 0) ? \
    R[0] : NULL)

#define __(R) \
  (*_(R))

#define reset(R) \
  memset(_(R), 0, sizeof(*_(R)));

ref(void) _salloc(size_t size, const char *type, void *placement);
void _sfree(ref(void) ptr, const char *file, size_t line);
int _svalid(ref(void) ptr, const char *file, size_t line);

/***************************************************
 * Vector
 ***************************************************/

#define vector(T) \
  T ***

#define vector_new(T) \
  (vector(T))_vector_new(sizeof(T), "vector("#T")")

#define vector_delete(V) \
  do \
  { \
    memcmp(&V, &V, 0); \
    memcmp(V[0], V[0], 0); \
    memcmp(V[0][0], V[0][0], 0); \
    _(V); \
    _vector_delete((vector(void))V, __FILE__, __LINE__); \
  } \
  while(0)

#define vector_size(V) \
  ((1 || \
    memcmp(&V, &V, 0) || \
    memcmp(V[0], V[0], 0) || \
    memcmp(V[0][0], V[0][0], 0)) ? \
    _vector_size((vector(void))V) : 0)

#define vector_erase(V, I, N) \
  do \
  { \
    memcmp(&V, &V, 0); \
    memcmp(V[0], V[0], 0); \
    memcmp(V[0][0], V[0][0], 0); \
    _vector_erase((vector(void))V, I, N); \
  } \
  while(0)

#define vector_push_back(V, E) \
  do \
  { \
    memcmp(&V, &V, 0); \
    memcmp(V[0], V[0], 0); \
    memcmp(V[0][0], V[0][0], 0); \
    _vector_resize((vector(void))V, vector_size(V) + 1); \
    _(V)[0][vector_size(V) - 1] = E; \
  } \
  while(0)

#define vector_at(V, I) \
  (_(V)[0][_vector_valid((vector(void))V, (1 || memcmp(&V, &V, 0) ? I : 0))])

#define vector_insert(V, B, S, I, N) \
  do \
  { \
    memcmp(&V, &V, 0); \
    memcmp(V[0], V[0], 0); \
    memcmp(V[0][0], V[0][0], 0); \
    memcmp(&S, &S, 0); \
    memcmp(S[0], S[0], 0); \
    memcmp(S[0][0], S[0][0], 0); \
    if(V == S){} \
    _vector_insert((vector(void))V, B, (vector(void))S, I, N); \
  } \
  while(0)

vector(void) _vector_new(size_t size, const char *type);
void _vector_delete(vector(void) ptr, const char *file, size_t line);
size_t _vector_size(vector(void) ptr);
void _vector_resize(vector(void) ptr, size_t size);
size_t _vector_valid(vector(void) ptr, size_t idx);
void _vector_erase(vector(void) ptr, size_t idx, size_t num);

void _vector_insert(vector(void) ptr, size_t before,
  vector(void) source, size_t idx, size_t num);

#else

/***************************************************
 * Dummy
 ***************************************************/

#define ref(T) \
  T *

#define salloc(T) \
  (ref(T))calloc(1, sizeof(T))

#define salloc_placement(T, F) \
  F

#define sfree(R) \
  free(R)

#define _(R) \
  R

#define reset(R) \
  memset(_(R), 0, sizeof(*_(R)));

#define vector(T) \
  T **

#define vector_new(T) \
  (vector(T))_vector_new(sizeof(T))

#define vector_delete(V) \
  _vector_delete((vector(void))V)

#define vector_size(V) \
  _vector_size((vector(void))V)

#define vector_push_back(V, E) \
  do \
  { \
    _vector_resize((vector(void))V, vector_size(V) + 1); \
    _(V)[0][vector_size(V) - 1] = E; \
  } \
  while(0)

#define vector_at(V, I) \
   (_(V)[0][_vector_valid((vector(void))V, I)])

#define vector_erase(V, I, N) \
  _vector_erase((vector(void))V, I, N);

#define vector_insert(V, B, S, I, N) \
  _vector_insert((vector(void))V, B, (vector(void))S, I, N)

vector(void) _vector_new(size_t size);
void _vector_delete(vector(void) ptr);
size_t _vector_size(vector(void) ptr);
void _vector_resize(vector(void) ptr, size_t size);
size_t _vector_valid(vector(void) ptr, size_t idx);
void _vector_erase(vector(void) ptr, size_t idx, size_t num);

void _vector_insert(vector(void) ptr, size_t before,
  vector(void) source, size_t idx, size_t num);

#endif

/***************************************************
 * Exceptions
 ***************************************************/

struct Exception
{
  int type;
  char message[STENT_EXCEPTION_MESSAGE_LENGTH];
};

#define scatch(E, F, P) \
  if(_scatch(&E, F, (ref(void))P)) \

void sthrow(int type, const char *message);
int _scatch(struct Exception *ex, void (*func)(ref(void)), ref(void) ptr);

#endif
