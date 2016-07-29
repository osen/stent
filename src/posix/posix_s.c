#include <posix_s.h>

#include <stdio.h>

object(FILE_s)
{
  FILE *raw;
};

static void popen_s_finalizer(ref(FILE_s) ctx)
{
  if(get(ctx)->raw)
  {
    pclose(get(ctx)->raw);
  }
}

ref(FILE_s) popen_s(const char *command, const char *mode)
{
  ref(FILE_s) rtn = {0};

  rtn = create(FILE_s);
  finalizer(rtn, popen_s_finalizer);
  get(rtn)->raw = popen(command, mode);

  if(!get(rtn)->raw)
  {
    throw(0, "Failed to open process");
  }

  return rtn;
}

void pclose_s(ref(FILE_s) ctx)
{
  int errorCode = 0;

  if(!get(ctx)->raw)
  {
    throw(0, "File descriptor no longer valid");
  }

  errorCode = pclose(get(ctx)->raw);
  get(ctx)->raw = NULL;

  if(errorCode != 0)
  {
    throw(errorCode, "Process failed");
  }

  destroy(ctx);
}

size_t fread_s(ref(CharArray) str, ref(FILE_s) stream)
{
  char *arr;

  arr = CharArrayGet(str);

  return fread(arr, sizeof(*arr), CharArraySize(str), get(stream)->raw);
}
