/*****************************************************************************
 * stent.h
 *
 * TODO
 * - Ensure memcpy compile time checks are never actually executed
 * - _svalid doesn't really need to return an int?
 * - _svalid should ensure passed in pointer is within allocated blocks
 * - Can temporaries be supported?
 *****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef STENT_STENT_H
#define STENT_STENT_H

#include <stdlib.h>
#include <string.h>

/*****************************************************************************
 * STENT_ENABLE
 *
 * Compile time flag to enable the tool. If disabled then dummy operations are
 * used instead. Facilities such as vector(T) are always available.
 *****************************************************************************/
#ifndef STENT_DISABLE
  #define STENT_ENABLE
#endif

#define STENT_BLOCKSIZE 1024

#ifdef STENT_ENABLE

/*****************************************************************************
 * refvoid
 *
 * MACRO to describe a void reference. This is mostly a convenience type as
 * it is used to pass into the generic utility functions for the tool. It is
 * similar to the ref(T) MACRO but avoids the struct requirement.
 *****************************************************************************/
#define refvoid \
  char **

/*****************************************************************************
 * ref(T)
 *
 * MACRO to describe a pointer tracked by the tool. When enabled this type has
 * an extra indirection because it is stored within a structure which can be
 * subsequently checked for a deletion flag prior to use. To access the raw
 * pointer it needs an additional dereference (via the _(P) MACRO) in order
 * check or validity and then to obtain the final pointer location (first
 * element of the referenced structure). This is the key component of this
 * tool.
 *****************************************************************************/
#define ref(T) \
  struct T **

/*****************************************************************************
 * _(P)
 *
 * MACRO to pass the reference into the implementation along with the unit
 * file name and line number for debug purposes. The memcmp calls ensure that
 * the specified reference is of the correct format for the tool. The returned
 * value is the first element of the passed in parameter to ensure type safety
 * and avoiding the need to manually cast.
 *****************************************************************************/
#define _(R) \
  ((_assert_ref(R))[0][0])

#define _assert_ref(R) \
  ((_svalid((refvoid)R, __FILE__, __LINE__) || \
    memcmp(&R, &R, 0) || \
    /* TODO: Not required */ \
    /* memcmp(&R[0], &R[0], 0) || */ \
    memcpy(&R[0][0], &R[0][0], 0) || \
    1) ? R : NULL)

/*****************************************************************************
 * allocate(T)
 *
 * MACRO to obtain the size and name string of specified type. This is passed
 * into the utility function which does the actual allocation. The data is
 * then explicitly casted to the specified type to ensure that the entire
 * operation is type safe.
 *****************************************************************************/
#define allocate(T) \
  (ref(T))_stent_alloc(sizeof(struct T), #T, __FILE__, __LINE__)

/*****************************************************************************
 * release(P)
 *
 * MACRO to ensure that specified pointer is in the correct format for the
 * tool and that it is not a temporary. The specified reference is then casted
 * and passed into the implementation along with file name of the unit and the
 * line number for debug purposes.
 *****************************************************************************/
#define release(R) \
  _stent_free((refvoid)_assert_ref(R), __FILE__, __LINE__);

/*****************************************************************************
 * cast
 *
 * MACRO to pass the type string and reference to check into the
 * implementation. Also pass through the unit file name and line number for
 * debug purposes. The returned refvoid is casted to the requested type if
 * the implementation did not cause an incorrect type error.
 *****************************************************************************/
#define cast(T, R) \
  (ref(T))_stent_cast(#T, (refvoid)_assert_ref(R), __FILE__, __LINE__)

/*****************************************************************************
 * void_cast
 *
 * TODO
 *****************************************************************************/
#define void_cast(R) \
  _stent_cast("void", (refvoid)_assert_ref(R), __FILE__, __LINE__)

refvoid _stent_alloc(size_t size, const char *type, const char *file, int line);
void _stent_free(refvoid ptr, const char *file, size_t line);

refvoid _stent_cast(const char *type, refvoid ptr,
  const char *file, size_t line);

int _svalid(refvoid ptr, const char *file, size_t line);

/***************************************************
 * Array
 ***************************************************/
size_t _array_check(size_t as, size_t es, size_t idx);

#define array_at(A, I) \
  (A[_array_check(sizeof(A), sizeof(A[0]), I)])

/***************************************************
 * Vector
 ***************************************************/

#define vector(T) \
  T ***

#define _assert_vector(V) \
  ((_svalid((refvoid)V, __FILE__, __LINE__) || \
    memcmp(&V, &V, 0) || \
    /* TODO: Not required */ \
    /* memcmp(&V[0], &V[0], 0) || */ \
    /* memcpy(&V[0][0], &V[0][0], 0) || */ \
    memcpy(&V[0][0][0], &V[0][0][0], 0) || \
    1) ? V : NULL)

#define vector_new(T) \
  (vector(T))_vector_new(sizeof(T), "vector("#T")", __FILE__, __LINE__)

#define vector_delete(V) \
  _vector_delete((vector(void))_assert_vector(V), __FILE__, __LINE__)

#define vector_size(V) \
  _vector_size((vector(void))_assert_vector(V))

#define vector_compare(V1, V2) \
  _vector_compare( \
    (vector(void))_assert_vector(V1), \
    (vector(void))_assert_vector(V2))

#define vector_erase(V, I, N) \
  _vector_erase((vector(void))_assert_vector(V), I, N)

#define vector_push_back(V, E) \
  do \
  { \
    _vector_resize((vector(void))_assert_vector(V), vector_size(V) + 1); \
    _(V)[vector_size(V) - 1] = E; \
  } \
  while(0)

#define vector_resize(V, S) \
  do \
  { \
    _vector_resize((vector(void))_assert_vector(V), S); \
  } \
  while(0)

#define vector_clear(V) \
  do \
  { \
    _vector_clear((vector(void))_assert_vector(V)); \
  } \
  while(0)

#define vector_at(V, I) \
  (_(V)[_vector_valid((vector(void))_assert_vector(V), I)])

#define vector_insert(V, B, S, I, N) \
  _vector_insert((vector(void))_assert_vector(V), B, (vector(void))_assert_vector(S), I, N)

vector(void) _vector_new(size_t size, const char *type, const char *file, int line);
void _vector_delete(vector(void) ptr, const char *file, size_t line);
size_t _vector_size(vector(void) ptr);
int _vector_compare(vector(void) ptr1, vector(void) ptr2);
void _vector_resize(vector(void) ptr, size_t size);
void _vector_clear(vector(void) ptr);
size_t _vector_valid(vector(void) ptr, size_t idx);
size_t _sstream_valid(vector(void) ptr, size_t idx);
void _vector_erase(vector(void) ptr, size_t idx, size_t num);

void _vector_insert(vector(void) ptr, size_t before,
  vector(void) source, size_t idx, size_t num);

/*
unsigned char sstream_at(ref(sstream) ctx, size_t idx);

#define sstream_at(S, I) \
  vector_at(_(S).data, I)

#define sstream_at(S, I) \
  (_(_(S).data)[_vector_valid((vector(void))_assert_vector(_(S).data), I + 1) - 1])
*/

#define sstream_at(S, I) \
  (_(_(S).data)[_sstream_valid((vector(void))_assert_vector(_(S).data), I)])

#else

/***************************************************
 * Dummy
 ***************************************************/

#define refvoid \
  char *

#define ref(T) \
  struct T *

#define allocate(T) \
  (ref(T))calloc(1, sizeof(struct T))

#define release(R) \
  free(R)

#define cast(T, R) \
  (ref(T))R

#define void_cast(R) \
  (refvoid)R

#define _(R) \
  R[0]

#define vector(T) \
  T **

#define vector_new(T) \
  (vector(T))_vector_new(sizeof(T))

#define vector_delete(V) \
  _vector_delete((vector(void))V)

#define vector_size(V) \
  _vector_size((vector(void))V)

#define vector_compare(V1, V2) \
  _vector_compare((vector(void))V1, (vector(void))V2)

#define vector_push_back(V, E) \
  do \
  { \
    _vector_resize((vector(void))V, vector_size(V) + 1); \
    _(V)[vector_size(V) - 1] = E; \
  } \
  while(0)

#define vector_resize(V, S) \
  _vector_resize((vector(void))V, S)

#define vector_clear(V) \
  _vector_clear((vector(void))V)

#define vector_at(V, I) \
   (_(V)[I])

#define vector_erase(V, I, N) \
  _vector_erase((vector(void))V, I, N)

#define vector_insert(V, B, S, I, N) \
  _vector_insert((vector(void))V, B, (vector(void))S, I, N)

vector(void) _vector_new(size_t size);
void _vector_delete(vector(void) ptr);
size_t _vector_size(vector(void) ptr);
int _vector_compare(vector(void) ptr1, vector(void) ptr2);
void _vector_resize(vector(void) ptr, size_t size);
void _vector_clear(vector(void) ptr);
/*
size_t _vector_valid(vector(void) ptr, size_t idx);
*/
void _vector_erase(vector(void) ptr, size_t idx, size_t num);

void _vector_insert(vector(void) ptr, size_t before,
  vector(void) source, size_t idx, size_t num);

#define array_at(A, I) (A[I])

#define sstream_at(S, I) \
   (vector_at(_(S).data, I))

#endif

/***************************************************
 * Both
 ***************************************************/
#define foreach(VAR, VEC, BODY) \
  { \
    size_t __var_i = 0; \
    size_t __var_size = vector_size(VEC); \
    for(; __var_i < __var_size; ++__var_i) \
    { \
      VAR = vector_at(VEC, __var_i); \
      BODY \
    } \
  }

#define vector_push(V, E) \
  vector_push_back(V, E)

#define vector_fill(V, S, N, D) \
  do \
  { \
    size_t __start = S; \
    size_t __num = N; \
    size_t vi = 0; \
    vector_at(V, __start) = D; \
    for(vi = __start; vi < __start + __num; ++vi) \
    { \
      vector_at(V, vi) = vector_at(V, __start); \
    } \
  } \
  while(0)

/***************************************************
 * String Stream
 ***************************************************/

/*
struct sstream;
*/
struct sstream
{
  vector(unsigned char) data;
};

ref(sstream) sstream_new();
ref(sstream) sstream_new_str(ref(sstream) other);
ref(sstream) sstream_new_cstr(const char *str);
void sstream_delete(ref(sstream) ctx);

void sstream_str_cstr(ref(sstream) ctx, const char *str);
void sstream_str(ref(sstream) ctx, ref(sstream) str);
void sstream_append(ref(sstream) ctx, ref(sstream) str);
void sstream_append_char(ref(sstream) ctx, unsigned char c);
void sstream_append_cstr(ref(sstream) ctx, const char *str);
void sstream_append_int(ref(sstream) ctx, int val);
void sstream_append_float(ref(sstream) ctx, float val);

int sstream_compare_cstr(ref(sstream) ctx, const char *str);
int sstream_compare(ref(sstream) ctx, ref(sstream) other);

void sstream_split(ref(sstream) ctx, unsigned char c, vector(ref(sstream)) out);
void sstream_split_eol(ref(sstream) ctx, vector(ref(sstream)) out);
char *sstream_cstr(ref(sstream) ctx);

size_t sstream_length(ref(sstream) ctx);
void sstream_erase(ref(sstream) ctx, size_t idx, size_t num);
void sstream_resize(ref(sstream) ctx, size_t length);

void sstream_insert(ref(sstream) dest, size_t dbegin, ref(sstream) source, size_t sbegin, size_t count);

vector(unsigned char) sstream_raw(ref(sstream) ctx);

int sstream_cmp_cstr(ref(sstream) ctx, const char *str);

/* TODO: Use sstream_str_cstr */
/* void sstream_clear(ref(sstream) ctx); */

/***************************************************
 * File Stream
 ***************************************************/

struct ifstream;

ref(ifstream) ifstream_popen_cstr(const char *cmd);
ref(ifstream) ifstream_popen(ref(sstream) cmd);
ref(ifstream) ifstream_open_cstr(const char *path);
ref(ifstream) ifstream_open(ref(sstream) path);
void ifstream_close(ref(ifstream) ctx);
int ifstream_eof(ref(ifstream) ctx);
void ifstream_getline(ref(ifstream) ctx, ref(sstream) out);
void ifstream_read(ref(ifstream) ctx, vector(unsigned char) out);

/***************************************************
 * Directory Handling
 ***************************************************/

struct dir;

ref(dir) dir_open_cstr(char *path);
ref(dir) dir_open(ref(sstream) path);
void dir_close(ref(dir) ctx);

/***************************************************
 * Error Handling
 ***************************************************/

void panic(const char *message);

#endif

#ifdef STENT_IMPLEMENTATION
#undef STENT_IMPLEMENTATION
/*****************************************************************************
 * STENT_IMPLEMENTATION
 *
 * This header is a combined definition and implementation. This means that
 * the implementation needs to be compiled once. The best practice is to
 * define STENT_IMPLEMENTATION and include stent.h once within the same unit
 * containing the main function. For the rest of the program, just include
 * stent.h as needed.
 *****************************************************************************/

#include <stdio.h>
#include <limits.h>

#ifdef STENT_ENABLE

/*****************************************************************************
 * Allocation
 *
 * Structure containing information about individual allocations. The ptr
 * must be the first element to allow the additional indirection of the
 * type-safe reference to work. The memory pointed to by ptr may get freed
 * but the Allocation structure itself persists throughout the lifespan of the
 * program.
 *****************************************************************************/
struct Allocation
{
  void *ptr;        /* Pointer to the native C memory block */
  int expired;      /* Track whether allocation has been freed */
  const char *type; /* The specified type for run-time type identification */
  const char *file; /* The source unit file the memory was allocated in */
  int line;         /* The line number the memory was allocated in */
};

/*****************************************************************************
 * Block
 *
 * Rather than allocate lots of small Allocation structures, allocations of
 * much larger blocks are done to reduce fragmentation. Each one of these
 * blocks tracks the count and then the next block in the list once it has
 * been filled.
 *****************************************************************************/
struct Block
{
  struct Allocation
    allocations[STENT_BLOCKSIZE]; /* Individual allocations */

  size_t count;                   /* The number of reserved allocations */
  struct Block *next;             /* The next block once this is exhausted */
};

/*****************************************************************************
 * blocks
 *
 * The head of the blocks list. This is assigned an initial block during the
 * _stent_init function. If an existing head exists, it is attached to the new
 * block causing the most recent allocations to be the fastest to find (though
 * searching in this way is typically never needed).
 *****************************************************************************/
static struct Block *blocks;

/*****************************************************************************
 * _stent_atexit
 *
 * This function is called when the program terminates. It iterates through
 * all the blocks in the list and then iterates through each of the individual
 * allocations. If any of these have not been freed before program exit it
 * reports a leak.
 *****************************************************************************/
static void _stent_atexit(void)
{
  struct Block *sb = NULL;
  size_t ai = 0;
  int doAbort = 0;

  sb = blocks;

  while(sb)
  {
    for(ai = 0; ai < sb->count; ai++)
    {
      if(!sb->allocations[ai].expired)
      {
        fprintf(stderr,
          "Warning: Allocated memory [%s:%i] persisted after application exit [%s]\n",
          sb->allocations[ai].file,
          sb->allocations[ai].line,
          sb->allocations[ai].type);

        doAbort = 1;
      }
    }

    sb = sb->next;
  }

  if(doAbort)
  {
    abort();
  }
}

/*****************************************************************************
 * _stent_init
 *
 * This function is called by every function relating to memory to ensure
 * that the initial block is not NULL. It simply allocates a new Block
 * structure and reports that this tool is active in the program.
 *****************************************************************************/
static void _stent_init()
{
  if(blocks)
  {
    return;
  }

  blocks = (struct Block *)calloc(1, sizeof(*blocks));

  if(!blocks)
  {
    fprintf(stderr, "Error: Failed to initialize initial block\n");

    abort();
  }

  fprintf(stderr, "Warning: Debug memory allocator enabled\n");
  atexit(_stent_atexit);
}

/*****************************************************************************
 * _stent_alloc
 *
 * Ensure the tool has been initialized and early return if a size of zero
 * has been specified. Assign the return value with the memory location of
 * the next free allocation. Allocate the native block of memory, if this
 * succeeds, increment the current block's allocation count (allocating a new
 * block if necessary) because this allocation is now reserved. Set additional
 * properties on the allocation such as type and return the value casted to
 * refvoid.
 *****************************************************************************/
refvoid _stent_alloc(size_t size, const char *type, const char *file, int line)
{
  struct Allocation *rtn = NULL;
  struct Block *sb = NULL;

  _stent_init();

  if(!size)
  {
    fprintf(stderr, "Warning: Allocation of zero size\n");

    return NULL;
  }

  rtn = &blocks->allocations[blocks->count];
  rtn->ptr = calloc(1, size);

  if(!rtn->ptr)
  {
    fprintf(stderr, "Error: Failed to allocate %s\n", type);
    abort();
  }

  blocks->count++;

  if(blocks->count >= STENT_BLOCKSIZE)
  {
    fprintf(stderr, "Warning: Adding allocation blocks\n");
    sb = (struct Block *)calloc(1, sizeof(*blocks));
    sb->next = blocks;
    blocks = sb;
  }

  rtn->type = type;
  rtn->file = file;
  rtn->line = line;

  return (refvoid)rtn;
}

/*****************************************************************************
 * _stent_free
 *
 * Ensure the tool has been initialized and that the specified allocation
 * referenced by ptr is still valid. Cast this to an Allocation structure
 * and free the native memory assigned to it. Finally set the expired flag so
 * that it is no longer seen as valid.
 *****************************************************************************/
void _stent_free(refvoid ptr, const char *file, size_t line)
{
  struct Allocation *allocation = NULL;

  _svalid(ptr, file, line);

  allocation = (struct Allocation *)ptr;
  free(allocation->ptr);
  allocation->expired = 1;
}

/*****************************************************************************
 * _stent_cast
 *
 * Ensure the tool has been initialized and that the specified allocation
 * referenced by ptr is still valid. Obtain the allocation and ensure that the
 * type string matches the one specified. If it matches then the cast is
 * assumed to be safe.
 *****************************************************************************/
refvoid _stent_cast(const char *type, refvoid ptr,
  const char *file, size_t line)
{
  struct Allocation *allocation = NULL;

  _svalid(ptr, file, line);

  allocation = (struct Allocation *)ptr;

  if(strcmp(type, "void") == 0)
  {
    return ptr;
  }

  if(strcmp(allocation->type, type) != 0)
  {
    fprintf(stderr,
      "Error: Attempt to cast [%s] to incompatible type [%s]\n",
      allocation->type, type);

    abort();
  }

  return ptr;
}

/*****************************************************************************
 * _svalid
 *
 * Ensure the tool has been initialized, check that the specified pointer
 * is not NULL and check if the obtained allocation has the expired flag set.
 * If the flag has not been set, assume the data is valid and return 1.
 *****************************************************************************/
int _svalid(refvoid ptr, const char *file, size_t line)
{
  struct Allocation *allocation = (struct Allocation *)ptr;

  /* TODO: Can ptr be anything but NULL if stent is not initialized? */
  /* _stent_init(); */

  if(!ptr)
  {
    fprintf(stderr, "Error: NULL pointer in %s %i\n",
      file, (int)line);

    abort();
  }

  /*
   * TODO: Check to see if ptr is within a block.
   */

  /*
   * TODO: Would allocation->ptr ever be NULL?
   */

  if(allocation->expired)
  {
    fprintf(stderr, "Error: %s pointer no longer valid in %s %i\n",
      allocation->type, file, (int)line);

    abort();
  }

  return 1;
}

/*****************************************************************************
 * _array_check
 *
 * TODO: Pass through file and line number.
 *****************************************************************************/
size_t _array_check(size_t as, size_t es, size_t idx)
{
  if(es * idx >= as)
  {
    fprintf(stderr, "Error: Index [index=%lu] out of bounds [size=%lu]\n",
      (unsigned long)idx, (unsigned long)as / es);

    abort();
  }

  return idx;
}

#endif

/***************************************************
 * Vector
 ***************************************************/

struct _StentVector
{
  unsigned char *data;
  size_t size;
  size_t allocated;
  size_t elementSize;
};

#ifdef STENT_ENABLE
vector(void) _vector_new(size_t size, const char *type, const char *file, int line)
#else
vector(void) _vector_new(size_t size)
#endif
{
  ref(_StentVector) rtn = NULL;

#ifdef STENT_ENABLE
  rtn = (ref(_StentVector))_stent_alloc(sizeof(struct _StentVector), type, file, line);
#else
  rtn = allocate(_StentVector);
#endif

  _(rtn).elementSize = size;

  return (vector(void))rtn;
}

#ifdef STENT_ENABLE
void _vector_delete(vector(void) ptr, const char *file, size_t line)
#else
void _vector_delete(vector(void) ptr)
#endif
{
  ref(_StentVector) v = NULL;

  v = (ref(_StentVector))ptr;
  free(_(v).data);

#ifdef STENT_ENABLE
  _stent_free((refvoid)ptr, file, line);
#else
  release(ptr);
#endif
}

size_t _vector_size(vector(void) ptr)
{
  ref(_StentVector) v = NULL;

  v = (ref(_StentVector))ptr;

  return _(v).size;
}

int _vector_compare(vector(void) ptr1, vector(void) ptr2)
{
  ref(_StentVector) v1 = NULL;
  ref(_StentVector) v2 = NULL;

  v1 = (ref(_StentVector))ptr1;
  v2 = (ref(_StentVector))ptr2;

  if(_(v1).size != _(v2).size)
  {
    return 1;
  }

  return memcmp(_(v1).data, _(v2).data, _(v1).size);
}

void _vector_clear(vector(void) ptr)
{
  ref(_StentVector) v = NULL;

  v = (ref(_StentVector))ptr;

  _(v).size = 0;
}

void _vector_resize(vector(void) ptr, size_t size)
{
  ref(_StentVector) v = NULL;
  size_t s = 0;
  unsigned char *d = NULL;

  v = (ref(_StentVector))ptr;

  if(_(v).allocated >= size)
  {
#ifdef STENT_ENABLE
    /*
     * Fill reclaimed space with zero
     */
    if(_(v).size < size)
    {
      memset(_(v).data + _(v).size * _(v).elementSize, 0, (size - _(v).size) * _(v).elementSize);
    }
#endif

    _(v).size = size;
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

  d = (unsigned char *)calloc(s, _(v).elementSize);

  if(!d)
  {
    fprintf(stderr, "Error: Failed to increase vector size\n");
    abort();
  }

  memcpy(d, _(v).data, _(v).elementSize * _(v).size);
  free(_(v).data);
  _(v).data = d;
  _(v).allocated = s;
  _(v).size = size;
}

size_t _vector_valid(vector(void) ptr, size_t idx)
{
  ref(_StentVector) v = NULL;

  v = (ref(_StentVector))ptr;

  if(_(v).size > idx)
  {
    return idx;
  }

  fprintf(stderr, "Error: Index [index=%lu] out of bounds [size=%lu]\n",
    (unsigned long)idx, (unsigned long)_(v).size);
  abort();
}

size_t _sstream_valid(vector(void) ptr, size_t idx)
{
  /* Check we are not overflowing into the '\0' terminator */
  _vector_valid(ptr, idx + 1);

  return _vector_valid(ptr, idx);
}

void _vector_erase(vector(void) ptr, size_t idx, size_t num)
{
  ref(_StentVector) v = NULL;
  char *dest = NULL;
  char *src = NULL;
  size_t tm = 0;

  v = (ref(_StentVector))ptr;

  if(idx >= _(v).size ||
    idx + num > _(v).size)
  {
    fprintf(stderr, "Error: Index out of bounds [size=%i] [index=%i] [num=%i]\n", (int)_(v).size, (int)idx, (int)num);
    abort();
  }

  if(!num)
  {
    return;
  }

  dest = (char *)_(v).data;
  dest += (idx * _(v).elementSize);

  src = dest;
  src += (num * _(v).elementSize);

  tm = (_(v).size - (idx + num)) * _(v).elementSize;

  if(tm)
  {
    memmove(dest, src, tm);
  }

  _(v).size -= num;
}

void _vector_insert(vector(void) ptr, size_t before,
  vector(void) source, size_t idx, size_t num)
{
  ref(_StentVector) s = NULL;
  ref(_StentVector) d = NULL;
  char *dest = NULL;
  char *src = NULL;
  size_t tm = 0;

  s = (ref(_StentVector))source;
  d = (ref(_StentVector))ptr;

  if(s == d)
  {
    fprintf(stderr, "Error: Source and desination must not match\n");
    abort();
  }

  if(!num)
  {
    return;
  }

  if(before > _(d).size)
  {
    fprintf(stderr, "Error: Invalid index specified. Non contiguous\n");
    abort();
  }

  if(idx >= _(s).size ||
    idx + num > _(s).size)
  {
    fprintf(stderr, "Error: Index out of bounds on source\n");
    abort();
  }

  tm = (_(d).size - before) * _(d).elementSize;

  _vector_resize(ptr, _(d).size + num);

  src = (char *)_(d).data;
  src += (before * _(d).elementSize);
  dest = src;
  dest += (num * _(d).elementSize);
  memmove(dest, src, tm);

  dest = src;
  src = (char *)_(s).data;
  src += (idx * _(d).elementSize);
  memcpy(dest, src, num * _(s).elementSize);
}

/***************************************************
 * String Stream
 ***************************************************/

/*
struct sstream
{
  vector(unsigned char) data;
};
*/

static char _scratch[256];

ref(sstream) sstream_new()
{
  ref(sstream) rtn = NULL;

  rtn = allocate(sstream);
  _(rtn).data = vector_new(unsigned char);
  vector_push_back(_(rtn).data, '\0');

  return rtn;
}

ref(sstream) sstream_new_str(ref(sstream) other)
{
  ref(sstream) rtn = sstream_new();

  sstream_str(rtn, other);

  return rtn;
}

ref(sstream) sstream_new_cstr(const char *str)
{
  ref(sstream) rtn = sstream_new();

  sstream_str_cstr(rtn, str);

  return rtn;
}

void sstream_delete(ref(sstream) ctx)
{
  vector_delete(_(ctx).data);
  release(ctx);
}

int sstream_compare_cstr(ref(sstream) ctx, const char *str)
{
  return strcmp(sstream_cstr(ctx), str);
}

int sstream_compare(ref(sstream) ctx, ref(sstream) other)
{
  return sstream_compare_cstr(ctx, sstream_cstr(other));
}

void sstream_append(ref(sstream) ctx, ref(sstream) str)
{
  size_t len = 0;
  size_t ci = 0;

  len = sstream_length(str);

  for(ci = 0; ci < len; ci++)
  {
    sstream_append_char(ctx, sstream_at(str, ci));
  }
}

void sstream_append_char(ref(sstream) ctx, unsigned char c)
{
  vector_at(_(ctx).data, vector_size(_(ctx).data) - 1) = c;
  vector_push_back(_(ctx).data, '\0');
}

void sstream_append_int(ref(sstream) ctx, int val)
{
  char str[(CHAR_BIT * sizeof(int) - 1) / 3 + 2] = {0};

  sprintf(str, "%i", val);
  sstream_append_cstr(ctx, str);
}

void sstream_append_float(ref(sstream) ctx, float val)
{
  /* TODO: Better size query */
  sprintf(_scratch, "%f", val);
  sstream_append_cstr(ctx, _scratch);
}

void sstream_append_cstr(ref(sstream) ctx, const char *str)
{
  size_t len = 0;
  size_t ci = 0;

  len = strlen(str);

  for(ci = 0; ci < len; ci++)
  {
    sstream_append_char(ctx, str[ci]);
  }
}

size_t sstream_length(ref(sstream) ctx)
{
  return vector_size(_(ctx).data) - 1;
}

/*
unsigned char sstream_at(ref(sstream) ctx, size_t idx)
{
  if(idx >= sstream_length(ctx))
  {
    panic("Character index out of range");
  }

  return vector_at(_(ctx).data, idx);
}
*/

void sstream_split(ref(sstream) ctx, unsigned char c, vector(ref(sstream)) out)
{
  ref(sstream) curr = NULL;
  size_t i = 0;
  unsigned char ch = 0;

  for(i = 0; i < sstream_length(ctx); i++)
  {
    ch = sstream_at(ctx, i);

    if(curr == NULL)
    {
      curr = sstream_new();
    }

    if(ch == c)
    {
      vector_push_back(out, curr);
      curr = NULL;
    }
    else
    {
      sstream_append_char(curr, ch);
    }
  }

  if(curr != NULL)
  {
    vector_push_back(out, curr);
  }
}

void sstream_split_eol(ref(sstream) ctx, vector(ref(sstream)) out)
{
  size_t max = 0;
  size_t i = 0;
  ref(sstream) curr = NULL;
  char ch = 0;

  /*
   * If out array already larger than 0, use that existing string
   * after blanking it.
   */
  if(max >= vector_size(out))
  {
    curr = sstream_new();
  }
  else
  {
    curr = vector_at(out, max);
    sstream_str_cstr(curr, "");
  }

  for(i = 0; i < sstream_length(ctx); i++)
  {
    ch = sstream_at(ctx, i);

    if(ch == '\n')
    {
      /*
       * Add to out array if not an empty token
       */
      if(sstream_length(curr) > 0)
      {
        /*
         * String is already in array if max is still smaller than array
         */
        if(max >= vector_size(out))
        {
          vector_push_back(out, curr);
        }
        max++;

        /*
         * If out array already larger than max, use that existing string
         * after blanking it.
         */
        if(max >= vector_size(out))
        {
          curr = sstream_new();
        }
        else
        {
          curr = vector_at(out, max);
          sstream_str_cstr(curr, "");
        }
      }
    }
    else if(ch == '\r')
    {
      /* Ignore */
    }
    else
    {
      sstream_append_char(curr, ch);
    }
  }

  /*
   * If remaining curr is not blank. Add it to out array
   * if not already reusing an element.
   */
  if(sstream_length(curr) > 0)
  {
    if(max >= vector_size(out))
    {
      vector_push_back(out, curr);
    }
    max++;
  }
  else
  {
    if(max >= vector_size(out))
    {
      sstream_delete(curr);
    }
  }

  /*
   * Erase the remaining elements. They are not needed.
   */
  while(vector_size(out) > max)
  {
    sstream_delete(vector_at(out, vector_size(out) - 1));
    vector_erase(out, vector_size(out) - 1, 1);
  }
}

char *sstream_cstr(ref(sstream) ctx)
{
  return (char *)&vector_at(_(ctx).data, 0);
}

void sstream_erase(ref(sstream) ctx, size_t idx, size_t num)
{
  if(idx + num > sstream_length(ctx))
  {
    fprintf(stderr, "Error: Selection reaches end of array\n");

    abort();
  }

  vector_erase(_(ctx).data, idx, num);
}

void sstream_resize(ref(sstream) ctx, size_t length)
{
  if(length == sstream_length(ctx))
  {
    return;
  }

  vector_resize(_(ctx).data, length + 1);
  vector_at(_(ctx).data, length) = '\0';
}

void sstream_str_cstr(ref(sstream) ctx, const char *str)
{
  vector_clear(_(ctx).data);
  vector_push_back(_(ctx).data, '\0');
  sstream_append_cstr(ctx, str);
}

void sstream_str(ref(sstream) ctx, ref(sstream) str)
{
  vector_clear(_(ctx).data);
  vector_push_back(_(ctx).data, '\0');
  sstream_append(ctx, str);
}

vector(unsigned char) sstream_raw(ref(sstream) ctx)
{
  return _(ctx).data;
}

void sstream_insert(ref(sstream) dest, size_t dbegin, ref(sstream) source, size_t sbegin, size_t count)
{
  if(dbegin > sstream_length(dest))
  {
    fprintf(stderr, "Error: Attempting to insert past end of stream\n");

    abort();
  }

  if(sbegin + count > sstream_length(source))
  {
    fprintf(stderr, "Error: Selection reaches end of stream\n");

    abort();
  }

  vector_insert(_(dest).data, dbegin, _(source).data, sbegin, count);
}

int sstream_cmp_cstr(ref(sstream) ctx, const char *str)
{
  return strcmp(sstream_cstr(ctx), str);
}

#ifndef _WIN32
/***************************************************
 * Directory Handling
 ***************************************************/
#include <dirent.h>

struct dir
{
  DIR *dp;
};

ref(dir) dir_open_cstr(char *path)
{
  ref(dir) rtn = NULL;
  DIR *dp = NULL;

  dp = opendir(path);

  if(!dp)
  {
    return NULL;
  }

  rtn = allocate(dir);
  _(rtn).dp = dp;

  return rtn;
}

ref(dir) dir_open(ref(sstream) path)
{
  return dir_open_cstr(sstream_cstr(path));
}

void dir_close(ref(dir) ctx)
{
  closedir(_(ctx).dp);
  release(ctx);
}
#endif

/***************************************************
 * File Stream
 ***************************************************/

struct ifstream
{
  FILE *fp;
  int pipe;
};

ref(ifstream) ifstream_open_cstr(const char *path)
{
  ref(ifstream) rtn = NULL;
  FILE *fp = NULL;

  fp = fopen(path, "rb");

  if(!fp)
  {
    return NULL;
  }

  rtn = allocate(ifstream);
  _(rtn).fp = fp;

  return rtn;
}

ref(ifstream) ifstream_popen_cstr(const char *cmd)
{
  ref(ifstream) rtn = NULL;
  FILE *fp = NULL;

#ifdef WIN32
  fp = _popen(cmd, "r");
#else
  fp = popen(cmd, "r");
#endif

  if(!fp)
  {
    return NULL;
  }

  rtn = allocate(ifstream);
  _(rtn).fp = fp;
  _(rtn).pipe = 1;

  return rtn;
}

ref(ifstream) ifstream_open(ref(sstream) path)
{
  return ifstream_open_cstr(sstream_cstr(path));
}

ref(ifstream) ifstream_popen(ref(sstream) cmd)
{
  return ifstream_popen_cstr(sstream_cstr(cmd));
}

void ifstream_close(ref(ifstream) ctx)
{
  if(_(ctx).pipe)
  {
#ifdef WIN32
    _pclose(_(ctx).fp);
#else
    pclose(_(ctx).fp);
#endif
  }
  else
  {
    fclose(_(ctx).fp);
  }

  release(ctx);
}

int ifstream_eof(ref(ifstream) ctx)
{
  int c = 0;

  if(feof(_(ctx).fp))
  {
    return 1;
  }

  c = getc(_(ctx).fp);

  if(c == EOF)
  {
    return 1;
  }

  ungetc(c, _(ctx).fp);

  return 0;
}

void ifstream_read_contents(ref(ifstream) ctx, vector(unsigned char) out)
{
  unsigned char buf[1024] = {0};

  vector_clear(out);

  while(1)
  {
    size_t ci = 0;

    size_t len = fread(buf, sizeof(unsigned char), 1024, _(ctx).fp);

    if(!len)
    {
      break;
    }

    for(ci = 0; ci < len; ci++)
    {
      vector_push(out, buf[ci]);
    }
  }
}

void ifstream_read(ref(ifstream) ctx, vector(unsigned char) out)
{
  size_t len = fread(&vector_at(out, 0), sizeof(unsigned char), vector_size(out), _(ctx).fp);

  vector_resize(out, len);
}

void ifstream_getline(ref(ifstream) ctx, ref(sstream) out)
{
  char buf[1024] = {0};
  size_t ci = 0;
  char rc = 0;

  sstream_str_cstr(out, "");

  while(1)
  {
    if(!fgets(buf, sizeof(buf), _(ctx).fp))
    {
      if(sstream_length(out) > 0)
      {
        return;
      }

      fprintf(stderr, "Error: Failed to read from input stream\n");
      abort();
    }

    ci = 0;

    while(1)
    {
      rc = buf[ci];

      if(rc == '\n')
      {
        return;
      }
      else if(rc == '\0')
      {
        break;
      }
      else if(rc == '\r')
      { }
      else
      {
        sstream_append_char(out, rc);
      }

      ci++;
    }
  }
}

void panic(const char *message)
{
  printf("Panic: %s\n", message);

  abort();
}

#endif

#ifdef __cplusplus
}
#endif

