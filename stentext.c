#include "stentext.h"

#include <string.h>
#include <stdio.h>

struct String
{
  char *data;
  size_t length;
};

void StringFree(REF(String) ctx)
{
  free(GET(ctx)->data);
}

REF(String) StringEmpty()
{
  REF(String) rtn = {};

  rtn = CALLOC(String);
  FINALIZER(rtn, StringFree);

  GET(rtn)->data = calloc(GET(rtn)->length + 1, sizeof(char));

  return rtn;
}

REF(String) StringAllocCStr(char *str)
{
  REF(String) rtn = {};

  rtn = CALLOC(String);
  FINALIZER(rtn, StringFree);
  GET(rtn)->length = strlen(str);
  GET(rtn)->data = calloc(GET(rtn)->length + 1, sizeof(char));

#ifdef __OpenBSD__
  strlcpy(GET(rtn)->data, str, GET(rtn)->length + 1);
#else
  strcpy(GET(rtn)->data, str);
#endif

  return rtn;
}

void StringCopy(REF(String) dest, REF(String) src)
{
  GET(dest)->length = GET(src)->length;
  free(GET(dest)->data);
  GET(dest)->data = strdup(GET(src)->data);
}

void StringAdd(REF(String) ctx, REF(String) add)
{
  GET(ctx)->length += GET(add)->length;
  GET(ctx)->data = realloc(GET(ctx)->data, sizeof(char) * (GET(ctx)->length + 1));
  strcat(GET(ctx)->data, GET(add)->data);
}

void StringAddCStr(REF(String) ctx, char *add)
{
  GET(ctx)->length += strlen(add);
  GET(ctx)->data = realloc(GET(ctx)->data, sizeof(char) * (GET(ctx)->length + 1));
  strcat(GET(ctx)->data, add);
}

void StringAddInt(REF(String) ctx, int add)
{
  // TODO: Get correct size
  char str[15] = {};

  sprintf(str, "%d", add);
  GET(ctx)->length += strlen(str);
  GET(ctx)->data = realloc(GET(ctx)->data, sizeof(char) * (GET(ctx)->length + 1));
  strcat(GET(ctx)->data, str);
}

char *StringCStr(REF(String) ctx)
{
  return GET(ctx)->data;
}

void StringAddChar(REF(String) ctx, char c)
{
  GET(ctx)->data = realloc(GET(ctx)->data, sizeof(char) * (GET(ctx)->length + 2));
  GET(ctx)->length++;
  GET(ctx)->data[GET(ctx)->length - 1] = c;
  GET(ctx)->data[GET(ctx)->length] = '\0';
}

void StringClear(REF(String) ctx)
{
  GET(ctx)->length = 0;
  GET(ctx)->data[0] = '\0';
}

size_t StringLength(REF(String) ctx)
{
  return GET(ctx)->length;
}

struct InputFile
{
  FILE *file;
  int eof;
};

void InputFileFinalize(REF(InputFile) ctx)
{
  if(GET(ctx)->file != NULL)
  {
    fclose(GET(ctx)->file);
  }
}

void InputFileReadLine(REF(InputFile) ctx, REF(String) out)
{
  char buffer[128] = {};
  char *res = NULL;
  size_t len = 0;
  int i = 0;

  if(InputFileEof(ctx))
  {
    THROW(0, "Attempt to read past end of file");
  }

  while(1)
  {
    res = fgets(buffer, sizeof(buffer), GET(ctx)->file);

    if(res == NULL)
    {
      GET(ctx)->eof = 1;
      return;
    }

    len = strlen(res);

    for(i = 0; i < len; i++)
    {
      if(buffer[i] == '\n')
      {
        char test = getc(GET(ctx)->file);

        if(test == EOF)
        {
          GET(ctx)->eof = 1;
        }
        else
        {
          ungetc(test, GET(ctx)->file);
        }

        return;
      }

      if(buffer[i] == '\r')
      {
        continue;
      }

      StringAddChar(out, buffer[i]);
    }
  }
}

REF(InputFile) InputFileOpen(char *path)
{
  REF(InputFile) rtn = {};

  rtn = CALLOC(InputFile);
  FINALIZER(rtn, InputFileFinalize);

  GET(rtn)->file = fopen(path, "r");

  if(GET(rtn)->file == NULL)
  {
    THROW(0, "Failed to open file");
  }

  return rtn;
}

int InputFileEof(REF(InputFile) ctx)
{
  return GET(ctx)->eof;
}

