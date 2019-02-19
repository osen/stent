#include <stdlib.h>

#define STENT_BLOCKSIZE 1024

/***************************************************
 * Reference
 ***************************************************/

#define ref(T) \
  T **

#define salloc(T) \
  (ref(T))_salloc(sizeof(T), #T)

#define sfree(R) \
  _sfree((void **)R, __FILE__, __LINE__)

#define _(R) \
  (_svalid((void **)R, __FILE__, __LINE__) ? R[0] : NULL)

void **_salloc(size_t size, char *type);
void _sfree(void **ptr, char *file, size_t line);
int _svalid(void **ptr, char *file, size_t line);

/***************************************************
 * Vector
 ***************************************************/

#define vector(T) \
  T ***

#define vector_new(T) \
  (vector(T))_vector_new(sizeof(T), "vector("#T")")

#define vector_delete(V) \
  _vector_delete(((void ****)&V)[0], (void **)V[0], (void *)V[0][0], \
    __FILE__, __LINE__)

#define vector_size(V) \
  _vector_size(((void ****)&V)[0], (void **)V[0], (void *)V[0][0])

#define vector_push_back(V, E) \
  do \
  { \
    _vector_resize(((void ****)&V)[0], (void **)V[0], (void *)V[0][0], \
      vector_size(V) + 1); \
    V[0][0][vector_size(V) - 1] = E; \
  } \
  while(0)

void ***_vector_new(size_t size, char *type);
void _vector_delete(void ***ptr, void **d1, void *d2, char *file, size_t line);
size_t _vector_size(void ***ptr, void **d1, void *d2);
void _vector_resize(void ***ptr, void **d1, void *d2, size_t size);
