#include <stdlib.h>

#define STENT_BLOCKSIZE 1024

#define ref(T) \
  T **

#define palloc(T) \
  (ref(T))_salloc(sizeof(T), #T)

#define pfree(R) \
  _sfree((void **)R, __FILE__, __LINE__)

#define _(R) \
  (_svalid((void **)R, __FILE__, __LINE__) ? R[0] : NULL)

void **_salloc(size_t size, char *type);
void _sfree(void **ptr, char *file, size_t line);
int _svalid(void **ptr, char *file, size_t line);
