#define STENT_IMPLEMENTATION
#include <stent.h>

#include <stdio.h>

int main()
{
  ref(ifstream) input = NULL;
  ref(sstream) line = NULL;

  input = ifstream_open_cstr("CMakeCache.txt");

  if(!input)
  {
    input = ifstream_open_cstr("CMakeLists.txt");
  }

  if(!input)
  {
    abort();
  }

  line = sstream_new();

  while(!ifstream_eof(input))
  {
    ifstream_getline(input, line);

    printf("Line: [%s]\n", sstream_cstr(line));
  }

  sstream_delete(line);
  ifstream_close(input);

  return 0;
}
