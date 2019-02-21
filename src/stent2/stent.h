#include <stdlib.h>
#include <string.h>

#include <stdio.h>

/*
#define STENT_ENABLE
*/
#define STENT_ENABLE

#define STENT_BLOCKSIZE 1024

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
  _sfree((void **)R, __FILE__, __LINE__, 1 || \
    memcmp(&R, &R, 0) || \
    memcmp(R[0], R[0], 0))

#define _(R) \
  (_svalid((void **)R, __FILE__, __LINE__, 1 || \
    memcmp(&R, &R, 0) || \
    memcmp(R[0], R[0], 0)) ? \
    R[0] : NULL)

void **_salloc(size_t size, const char *type, void *placement);
void _sfree(void **ptr, const char *file, size_t line, int dummy);
int _svalid(void **ptr, const char *file, size_t line, int dummy);

/***************************************************
 * Vector
 ***************************************************/

#define vector(T) \
  T ***

#define vector_new(T) \
  (vector(T))_vector_new(sizeof(T), "vector("#T")")

#define vector_delete(V) \
  _vector_delete((void ***)V, __FILE__, __LINE__, 1 || \
    memcmp(&V, &V, 0) || \
    memcmp(V[0], V[0], 0) || \
    memcmp(V[0][0], V[0][0], 0))

#define vector_size(V) \
  _vector_size((void ***)V, 1 || \
    memcmp(&V, &V, 0) || \
    memcmp(V[0], V[0], 0) || \
    memcmp(V[0][0], V[0][0], 0))

#define vector_push_back(V, E) \
  do \
  { \
    _vector_resize((void ***)V, vector_size(V) + 1, 1 || \
    memcmp(&V, &V, 0) || \
    memcmp(V[0], V[0], 0) || \
    memcmp(V[0][0], V[0][0], 0)); \
    V[0][0][vector_size(V) - 1] = E; \
  } \
  while(0)

#define vector_at(V, I) \
   ((&V == &V) ? _(V)[0][_vector_valid((void ***)V, I)] : _(V)[0][0])

void ***_vector_new(size_t size, const char *type);
void _vector_delete(void ***ptr, const char *file, size_t line, int dummy);
size_t _vector_size(void ***ptr, int dummy);
void _vector_resize(void ***ptr, size_t size, int dummy);
size_t _vector_valid(void ***ptr, size_t idx);

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

#endif
