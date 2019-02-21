#include <stdlib.h>
#include <string.h>

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
    _vector_delete((vector(void))V, __FILE__, __LINE__); \
  } \
  while(0)

#define vector_size(V) \
  ((1 || \
    memcmp(&V, &V, 0) || \
    memcmp(V[0], V[0], 0) || \
    memcmp(V[0][0], V[0][0], 0)) ? \
    _vector_size((vector(void))V) : 0)

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
   ((&V == &V) ? _(V)[0][_vector_valid((vector(void))V, I)] : _(V)[0][0])

vector(void) _vector_new(size_t size, const char *type);
void _vector_delete(vector(void) ptr, const char *file, size_t line);
size_t _vector_size(vector(void) ptr);
void _vector_resize(vector(void) ptr, size_t size);
size_t _vector_valid(vector(void) ptr, size_t idx);

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

vector(void) _vector_new(size_t size);
void _vector_delete(vector(void) ptr);
size_t _vector_size(vector(void) ptr);
void _vector_resize(vector(void) ptr, size_t size);
size_t _vector_valid(vector(void) ptr, size_t idx);

#endif
