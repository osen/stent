#include "stent.h"

#include <string.h>
#include <stdlib.h>
#include <setjmp.h>

#define INITIAL_ALLOCATION_SIZE 8

struct Allocation
{
  void *ptr;
  time_t time;
  size_t unique;
  char *type;
  char *file;
  int line;
  void (*finalizeFunc)(ref(Object));
  int destroying;
  int finalizing;
  ref(Object) **strongs;
  size_t strongCount;
  int exceptionStackLocation;
};

struct ExceptionStackItem
{
  jmp_buf buf;
  int code;
  char *message;
  char *file;
  int line;
};

static struct
{
  ref(Object) tempObject;
  struct Allocation **allocations;
  size_t allocationCount;
  time_t currentTime;
  size_t unique;

  struct ExceptionStackItem *exceptionStack;
  size_t exceptionStackSize;
  size_t exceptionStackLocation;

  size_t destroyDepth;
} stent;

static ref(Object) *_stent_object_p_from_allocation_idx(size_t idx);

void stent_cleanup()
{
  size_t i;
  struct Allocation *allocation;
  ref(Object) obj;

  /* Iterate through left over allocations and destroy any
   * lingering reference whilst reporting it as a leak. */
  for(i = 0; i < stent.allocationCount; i++)
  {
    allocation = stent.allocations[i];

    if(!allocation)
    {
      break;
    }

    if(!allocation->ptr)
    {
      continue;
    }

    printf("\n");
    printf("***** Memory Leak *****\n");
    printf("Type: %s\n", allocation->type);
    printf("File: %s\n", allocation->file);
    printf("Line: %i\n", allocation->line);

    obj = *_stent_object_p_from_allocation_idx(i);
    _stent_destroy(obj, 0);
    /* destroy(obj); */
  }

  /* Deallocate the memory allocated by stent and reset back to
   * a default state ready to be exited or reused. */
  for(i = 0; i < stent.allocationCount; i++)
  {
    allocation = stent.allocations[i];

    if(!allocation)
    {
      break;
    }

    free(allocation);
  }

  free(stent.allocations);
  free(stent.exceptionStack);
  memset(&stent, 0, sizeof(stent));
}

int _stent_throw_if_not_func(void (*a)(), void (*b)())
{
  if(a != b)
  {
    throw(0, "Reference not yet initialized");
  }

  return 0;
}

static void _stent_finalize(int pass)
{
  size_t i;
  struct Allocation *allocation;
  ref(Object) obj;
  int found;
  size_t newest;

  /* Keep looping through allocations until none remain to be
   * finalized or deallocated. */
  while(1)
  {
    found = 0;

    /* HACK: To fix -pedantic warning. */
    newest = 0;

    for(i = 0; i < stent.allocationCount; i++)
    {
      allocation = stent.allocations[i];

      if(!allocation)
      {
        break;
      }

      if(!allocation->ptr)
      {
        continue;
      }

      if(!allocation->destroying)
      {
        continue;
      }

      /* If on the finalize pass, set this flag so the same object
       * does not keep getting finalized. */
      if(pass == 0)
      {
        if(allocation->finalizing)
        {
          continue;
        }
      }

      /* Find the newest object possible ready for finalization or
       * deallocation. */
      if(!found)
      {
        newest = i;
        found = 1;
      }
      else
      {
        if(allocation->time > stent.allocations[newest]->time)
        {
          newest = i;
        }
        else if(allocation->time == stent.allocations[newest]->time &&
          allocation->unique > stent.allocations[newest]->unique)
        {
          newest = i;
        }
      }
    }

    if(!found)
    {
      break;
    }

    allocation = stent.allocations[newest];

    if(pass == 0)
    {
      /* If this is the first pass, then simply run the finalizer
       * if the object has one set. */
      allocation->finalizing = 1;

      if(allocation->finalizeFunc)
      {
        obj = *_stent_object_p_from_allocation_idx(newest);
        allocation->finalizeFunc(obj);
      }
    }
    else if(pass == 1)
    {
      /* If this is the second pass, then deallocate the memory
       * rquired by the allocation. */
      free(allocation->type);
      free(allocation->file);
      free(allocation->ptr);
      allocation->ptr = NULL;
    }
  }
}

void _stent_destroy(ref(Object) obj, int unused)
{
  struct Allocation *allocation;
  size_t i;

  if(!valid(obj))
  {
    throw(0, "Attempting to destroy invalid reference");
  }

  allocation = stent.allocations[obj.idx];

  if(allocation->destroying)
  {
    return;
  }

  stent.destroyDepth++;
  allocation->destroying = 1;

  /* If this reference has other references strongly depending on it
   * iterate through them and after ensuring they are valid, recursively
   * call this function on them, however we only finalize once at the end
   * of this "toplevel" function. */
  if(allocation->strongs)
  {
    for(i = 0; i < allocation->strongCount; i++)
    {
      if(!allocation->strongs[i])
      {
        break;
      }

      if(valid(*allocation->strongs[i]))
      {
        _stent_destroy(*allocation->strongs[i], 0);
      }

      free(allocation->strongs[i]);
    }

    free(allocation->strongs);
    allocation->strongs = NULL;
  }

  /* We don't want embedded finalizations or deallocations
   * so we only run the passes on the toplevel call to destroy.
   * we also don't want destroy to run these passes when called
   * from inside a finalizer. */
  if(stent.destroyDepth == 1)
  {
    _stent_finalize(0);
    _stent_finalize(1);
  }

  stent.destroyDepth--;
}

void *_stent_get(ref(Object) obj, int throws, int unused)
{
  struct Allocation *allocation;

  if(obj.idx >= stent.allocationCount)
  {
    if(!throws) return NULL;
    throw(0, "Allocation out of bounds");
  }

  allocation = stent.allocations[obj.idx];

  if(!allocation)
  {
    if(!throws) return NULL;
    throw(0, "Allocation does not exist");
  }

  if(!allocation->ptr)
  {
    if(!throws) return NULL;
    throw(0, "Allocation has expired");
  }

  if(allocation->ptr != obj.ptr)
  {
    if(!throws) return NULL;
    throw(0, "Allocation has invalid pointer");
  }

  if(allocation->time != obj.time)
  {
    if(!throws) return NULL;
    throw(0, "Allocation has invalid timestamp");
  }

  if(allocation->unique != obj.unique)
  {
    if(!throws) return NULL;
    throw(0, "Allocation has invalid sub-timestamp");
  }

  return allocation->ptr;
}

ref(Object) *_stent_object_p_from_null()
{
  memset(&stent.tempObject, 0, sizeof(stent.tempObject));

  *(void (**)())(&stent.tempObject._get) = (void (*)())_stent_get;
  *(void (**)())(&stent.tempObject._finalizer) = (void (*)())_stent_finalizer;
  *(void (**)())(&stent.tempObject._strong) = (void (*)())_stent_strong;
  *(void (**)())(&stent.tempObject._cast) = (void (*)())_stent_cast;
  *(void (**)())(&stent.tempObject._try) = (void (*)())_stent_try;

  return &stent.tempObject;
}

static ref(Object) *_stent_object_p_from_allocation_idx(size_t idx)
{
  struct Allocation *allocation;

  if(idx >= stent.allocationCount)
  {
    throw(0, "Allocation out of bounds");
  }

  allocation = stent.allocations[idx];

  if(allocation == NULL)
  {
    throw(0, "Allocation does not exist");
  }

  if(allocation->ptr == NULL)
  {
    throw(0, "Allocation has expired");
  }

  /* Initialize temporary ref(Object) with function pointers. */
  _stent_object_p_from_null();

  /* Initialize temporary ref(Object) with allocation specific data. */
  stent.tempObject.idx = idx;
  stent.tempObject.ptr = allocation->ptr;
  stent.tempObject.time = allocation->time;
  stent.tempObject.unique = allocation->unique;

  return &stent.tempObject;
}

static void _stent_array_finalizer(array(Object) ctx)
{
  //printf("~array(Object)\n");

  if(get(ctx)->data)
  {
    free(get(ctx)->data);
  }
}

ref(Object) *_stent_array_create(char *type, char *file, int line)
{
  ref(Object) *rtn;
  array(Object) tmp;
  array(Object) *ptmp;

  rtn = _stent_create(sizeof(array(Object)), type, file, line);
  tmp = *((array(Object) *)rtn);
  finalizer(tmp, _stent_array_finalizer);
  ptmp = (array(Object) *)rtn;
  *(void (**)())(&ptmp->ptr->_at) = (void (*)())_stent_array_at;

  return rtn;
}

ref(Object) _stent_array_at(array(Object) arr, size_t idx, int unused)
{
  if(idx < 0 || idx >= array_size(arr))
  {
    throw(0, "Array index out of bounds");
  }

  return get(arr)->data[idx];
}

ref(Object) *_stent_create(size_t size, char *type, char *file, int line)
{
  void *ptr;
  struct Allocation **allocations;
  struct Allocation *allocation;
  size_t i;
  time_t currentTime;

  ptr = calloc(1, size);

  /* If allocation failed, return zeroed object. */
  if(!ptr)
  {
    throw(0, "Failed to allocate space for object");
  }

  /* If allocations array is not yet initialized, initialize
   * it to default size. */
  if(!stent.allocations)
  {
    stent.allocations = calloc(INITIAL_ALLOCATION_SIZE,
      sizeof(*stent.allocations));

    if(!stent.allocations)
    {
      free(ptr);
      throw(0, "Failed to allocate space for allocations");
    }

    stent.allocationCount = INITIAL_ALLOCATION_SIZE;
  }

  /* Iterate through each allocation, obtain and zero out the first
   * allocation that was either NULL or had it's referring pointer
   * set to NULL. */
  allocation = NULL;

  for(i = 0; i < stent.allocationCount; i++)
  {
    if(stent.allocations[i] == NULL)
    {
      allocation = calloc(1, sizeof(**stent.allocations));

      if(!allocation)
      {
        free(ptr);
        throw(0, "Failed to allocate space for allocation");
      }

      stent.allocations[i] = allocation;
      break;
    }

    if(stent.allocations[i]->ptr == NULL)
    {
      allocation = stent.allocations[i];
      memset(allocation, 0, sizeof(*allocation));
      break;
    }
  }

  /* If no allocation reuse is possible then increase the size
   * of the allocations array. */
  if(!allocation)
  {
    allocations = realloc(stent.allocations,
      (stent.allocationCount * 2) * sizeof(*stent.allocations));

    if(!allocations)
    {
      free(ptr);
      throw(0, "Failed to allocate space for allocations");
    }

    stent.allocations = allocations;

    memset(stent.allocations + stent.allocationCount, 0,
      stent.allocationCount * sizeof(*stent.allocations));

    i = stent.allocationCount;
    stent.allocationCount *= 2;
    allocation = calloc(1, sizeof(**stent.allocations));

    if(!allocation)
    {
      free(ptr);
      throw(0, "Failed to allocate space for allocation");
    }

    stent.allocations[i] = allocation;
  }

  time(&currentTime);

  if(currentTime != stent.currentTime)
  {
    stent.currentTime = currentTime;
    stent.unique = 0;
  }

  /* Populate the allocation data with required information such
   * as source file it was allocated in, line number, pointer etc.*/
  allocation->type = strdup(type);

  if(allocation->type == NULL)
  {
    free(ptr);
    throw(0, "Failed to allocate type string");
  }

  allocation->file = strdup(file);

  if(allocation->file == NULL)
  {
    free(allocation->type);
    free(ptr);
    throw(0, "Failed to allocate file string");
  }

  allocation->line = line;
  allocation->ptr = ptr;
  allocation->time = stent.currentTime;
  allocation->unique = stent.unique;
  allocation->exceptionStackLocation = stent.exceptionStackLocation;

  stent.unique++;

  return _stent_object_p_from_allocation_idx(i);
}

void _stent_finalizer(ref(Object) obj, void (*func)(ref(Object)), int unused)
{
  struct Allocation *allocation;

  if(!valid(obj))
  {
    throw(0, "Attempting to add finalizer to invalid reference");
  }

  allocation = stent.allocations[obj.idx];
  allocation->finalizeFunc = func;
}

void _stent_strong(ref(Object) parent, ref(Object) child, int unused)
{
  struct Allocation *allocation;
  size_t i;
  ref(Object) *obj;
  ref(Object) **strongs;

  if(!valid(parent))
  {
    throw(0, "Attempting to add strong reference to invalid reference");
  }

  if(!valid(child))
  {
    throw(0, "Attempting to add invalid reference as strong reference");
  }

  allocation = stent.allocations[parent.idx];

  /* If strongs array is not yet initialized, initialize
   * it to default size. */
  if(!allocation->strongs)
  {
    allocation->strongs = calloc(INITIAL_ALLOCATION_SIZE,
      sizeof(*allocation->strongs));

    if(!allocation->strongs)
    {
      throw(0, "Failed to allocate space for strong references");
    }

    allocation->strongCount = INITIAL_ALLOCATION_SIZE;
  }

  /* Iterate through each strong reference, obtain the first
   * ref that was either NULL or had become invalid. */
  obj = NULL;

  for(i = 0; i < allocation->strongCount; i++)
  {
    if(allocation->strongs[i] == NULL)
    {
      obj = calloc(1, sizeof(**allocation->strongs));

      if(!obj)
      {
        throw(0, "Failed to allocate space for strong reference");
      }

      allocation->strongs[i] = obj;
      break;
    }

    if(!valid(*allocation->strongs[i]))
    {
      obj = allocation->strongs[i];
      break;
    }
  }

  /* If no strong reference reuse is possible then increase the size
   * of the strong references array. */
  if(!obj)
  {
    strongs = realloc(allocation->strongs,
      (allocation->strongCount * 2) * sizeof(*allocation->strongs));

    if(!strongs)
    {
      throw(0, "Failed to increase space for strong references");
    }

    allocation->strongs = strongs;

    memset(allocation->strongs + allocation->strongCount, 0,
      allocation->strongCount * sizeof(*allocation->strongs));

    i = allocation->strongCount;
    allocation->strongCount *= 2;
    obj = calloc(1, sizeof(**allocation->strongs));

    if(!obj)
    {
      throw(0, "Failed to allocate space for strong reference");
    }

    allocation->strongs[i] = obj;
  }

  *obj = child;
}

ref(Object) *_stent_cast(char *type, ref(Object) obj, int unused)
{
  struct Allocation *allocation;

  if(!valid(obj))
  {
    throw(0, "Object is not valid for type cast");
  }

  allocation = stent.allocations[obj.idx];

  if(strcmp(allocation->type, type) == 0 || strcmp("struct Object", type) == 0)
  {
    return _stent_object_p_from_allocation_idx(obj.idx);
  }

  return _stent_object_p_from_null();
}

static void _stent_release_refs(int exceptionLevel, int performFree)
{
  struct Allocation *allocation;
  size_t i;
  ref(Object) current;
  int found;
  size_t newest;

  if(!performFree)
  {
    for(i = 0; i < stent.allocationCount; i++)
    {
      allocation = stent.allocations[i];

      if(!allocation)
      {
        break;
      }

      if(!allocation->ptr)
      {
        continue;
      }

      current = *_stent_object_p_from_allocation_idx(i);

      if(!valid(current))
      {
        continue;
      }

      if(allocation->exceptionStackLocation == exceptionLevel)
      {
        allocation->exceptionStackLocation--;
      }
    }

    return;
  }
  else
  {
    while(1)
    {
      found = 0;

      /* HACK: To fix -pedantic warning. */
      newest = 0;

      for(i = 0; i < stent.allocationCount; i++)
      {
        allocation = stent.allocations[i];

        if(!allocation)
        {
          break;
        }

        if(!allocation->ptr)
        {
          continue;
        }

        if(allocation->exceptionStackLocation != exceptionLevel)
        {
          continue;
        }

        /* Find the newest object possible ready for finalization or
         * deallocation. */
        if(!found)
        {
          newest = i;
          found = 1;
        }
        else
        {
          if(allocation->time > stent.allocations[newest]->time)
          {
            newest = i;
          }
          else if(allocation->time == stent.allocations[newest]->time &&
            allocation->unique > stent.allocations[newest]->unique)
          {
            newest = i;
          }
        }
      }

      if(!found)
      {
        break;
      }

      current = *_stent_object_p_from_allocation_idx(newest);

      if(!valid(current))
      {
        continue;
      }

      destroy(current);
    }
  }
}

static void _stent_exception_finalizer(ref(Exception) ctx)
{
  if(get(ctx)->message)
  {
    free(get(ctx)->message);
  }

  if(get(ctx)->file)
  {
    free(get(ctx)->file);
  }
}

ref(Exception) _stent_try(void (*func)(ref(Object)), ref(Object) userData,
  int unused)
{
  ref(Exception) rtn;
  struct ExceptionStackItem *exceptionStack;
  struct ExceptionStackItem *esi;

  stent.exceptionStackLocation++;
  exceptionStack = stent.exceptionStack;

  if(stent.exceptionStackLocation >= stent.exceptionStackSize)
  {
    exceptionStack = realloc(exceptionStack,
      stent.exceptionStackLocation * sizeof(*exceptionStack));

    if(!exceptionStack)
    {
      stent.exceptionStackLocation--;
      throw(0, "Failed to allocate memory for exception stack");
    }

    stent.exceptionStackSize = stent.exceptionStackLocation;
    stent.exceptionStack = exceptionStack;
  }

  esi = &stent.exceptionStack[stent.exceptionStackLocation - 1];

  if(setjmp(esi->buf) == 0)
  {
    func(userData);
    _stent_release_refs(stent.exceptionStackLocation, 0);

    // If try called in finalizer, exceptionStack and esr may become invalid.
    esi = &stent.exceptionStack[stent.exceptionStackLocation - 1];
    stent.exceptionStackLocation--;

    rtn = nullref(Exception);
  }
  else
  {
    _stent_release_refs(stent.exceptionStackLocation, 1);
    // If try called in finalizer, exceptionStack and esr may become invalid.
    esi = &stent.exceptionStack[stent.exceptionStackLocation - 1];

    stent.exceptionStackLocation--;

    rtn = create(Exception);
    finalizer(rtn, _stent_exception_finalizer);
    get(rtn)->message = esi->message;
    get(rtn)->errorCode = esi->code;
    get(rtn)->file = esi->file;
    get(rtn)->line = esi->line;
  }

  return rtn;
}

void _stent_throw(int code, char *message, char *file, int line)
{
  struct ExceptionStackItem *esi;

  if(stent.exceptionStackLocation <= 0 || stent.destroyDepth > 0)
  {
    printf("Unhandled Exception (%i): %s\nFILE: %s\nLINE: %i\n",
      code, message, file, line);

    abort();
  }

  esi = &stent.exceptionStack[stent.exceptionStackLocation - 1];

  esi->code = code;
  esi->message = strdup(message);
  esi->file = strdup(file);
  esi->line = line;

  longjmp(esi->buf, 1);
}

char *ExceptionMessage(ref(Exception) ctx)
{
  return get(ctx)->message;
}

#define _stent_ARRAY_DEF(T, N) \
  object(N##Array) \
  { \
    T *data; \
    size_t size; \
  }; \
  \
  static void N##ArrayFinalizer(ref(N##Array) ctx) \
  { \
    if(get(ctx)->data) \
    { \
      free(get(ctx)->data); \
    } \
  } \
  ref(N##Array) N##ArrayCreate() \
  { \
    ref(N##Array) rtn; \
    \
    rtn = create(N##Array); \
    finalizer(rtn, N##ArrayFinalizer); \
    \
    return rtn; \
  } \
  void N##ArrayResize(ref(N##Array) ctx, size_t size) \
  { \
    void *tmp; \
    tmp = realloc(get(ctx)->data, sizeof(*get(ctx)->data) * size); \
    if(!tmp) throw(0, "Failed to increase size of array"); \
    get(ctx)->data = tmp; \
    get(ctx)->size = size; \
  } \
  T* N##ArrayGet(ref(N##Array) ctx) \
  { \
    return get(ctx)->data; \
  } \
  T N##ArrayAt(ref(N##Array) ctx, size_t idx) \
  { \
    if(idx < 0 || idx >= get(ctx)->size) \
    { \
      throw(0, "Array index out of bounds"); \
    } \
    return get(ctx)->data[idx]; \
  } \
  size_t N##ArraySize(ref(N##Array) ctx) \
  { \
    return get(ctx)->size; \
  } \
  void N##ArrayAdd(ref(N##Array) ctx, T toadd) \
  { \
    N##ArrayResize(ctx, get(ctx)->size + 1); \
    get(ctx)->data[get(ctx)->size - 1] = toadd; \
  } \
  void N##ArraySet(ref(N##Array) ctx, size_t idx, T toset) \
  { \
    if(idx < 0 || idx >= get(ctx)->size) \
    { \
      throw(0, "Index out of bounds"); \
    } \
    get(ctx)->data[idx] = toset; \
  }

_stent_ARRAY_DEF(char, Char)
_stent_ARRAY_DEF(size_t, Size_t)

object(String)
{
  ref(CharArray) data;
};

ref(String) StringCreate()
{
  ref(String) rtn;

  rtn = create(String);
  get(rtn)->data = CharArrayCreate();
  strong(rtn, get(rtn)->data);
  CharArrayAdd(get(rtn)->data, '\0');

  return rtn;
}

ref(String) StringFromCStr(char *str)
{
  ref(String) rtn;

  rtn = StringCreate();
  CharArrayResize(get(rtn)->data, CharArraySize(get(rtn)->data) + strlen(str));
  strcpy(CharArrayGet(get(rtn)->data), str);

  return rtn;
}

ref(String) StringFromSubstring(ref(String) str, size_t start, size_t length)
{
  size_t i;
  ref(String) rtn;

  if(start + length > StringLength(str))
  {
    throw(0, "String position out of bounds");
  }

  rtn = StringCreate();

  for(i = start; i < start + length; i++)
  {
    StringAddChar(rtn, StringAt(str, i));
  }

  return rtn;
}

char *StringCStr(ref(String) ctx)
{
  return CharArrayGet(get(ctx)->data);
}

void StringAddCStr(ref(String) ctx, char *str)
{
  size_t len;

  len = strlen(str);
  CharArrayResize(get(ctx)->data, CharArraySize(get(ctx)->data) + len);
  strcat(CharArrayGet(get(ctx)->data), str);
}

void StringAddChar(ref(String) ctx, char c)
{
  CharArraySet(get(ctx)->data, CharArraySize(get(ctx)->data) - 1, c);
  CharArrayAdd(get(ctx)->data, '\0');
}

void StringAddCharArray(ref(String) ctx, ref(CharArray) toadd, size_t start, size_t len)
{
  char *src;
  char *dest;
  size_t origSize;

  if(start < 0 ||
    len < 0 ||
    start + len >= CharArraySize(toadd))
  {
    throw(0, "Invalid bounds");
  }

  src = CharArrayGet(toadd) + start;
  origSize = CharArraySize(get(ctx)->data);
  CharArrayResize(get(ctx)->data, origSize + len);
  dest = CharArrayGet(get(ctx)->data) + origSize - 1;
  memcpy(dest, src, len * sizeof(char));
  dest[CharArraySize(get(ctx)->data) - 1] = '\0';
}

int StringCompareCStr(ref(String) ctx, char *str)
{
  return strcmp(str, StringCStr(ctx));
}

char StringAt(ref(String) ctx, size_t idx)
{
  if(idx < 0 || idx >= StringLength(ctx))
  {
    throw(0, "String position out of bounds");
  }

  return CharArrayAt(get(ctx)->data, idx);
}

size_t StringLength(ref(String) ctx)
{
  return CharArraySize(get(ctx)->data) - 1;
}

ref(CharArray) StringCharArray(ref(String) ctx)
{
  return get(ctx)->data;
}

void StringAddInt(ref(String) ctx, int val)
{
  char str[10];

  if(val >= 100000000)
  {
    throw(0, "Support for large values not yet implemented");
  }

  snprintf(str, 10, "%i", val);
  StringAddCStr(ctx, str);
}
