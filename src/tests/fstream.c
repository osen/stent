#define STENT_IMPLEMENTATION
#include <stent.h>

#include <stdio.h>

int main()
{
  ref(ifstream) input = NULL;
  ref(sstream) line = NULL;
  ref(sstream) total = NULL;
  vector(unsigned char) data = NULL;

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
  total = sstream_new();

  while(!ifstream_eof(input))
  {
    ifstream_getline(input, line);
    printf("Line: [%s]\n", sstream_cstr(line));
    sstream_append(total, line);
  }

  printf("Total: [%s]\n", sstream_cstr(total));

  sstream_delete(total);
  sstream_delete(line);
  ifstream_close(input);

  input = ifstream_open_cstr("CMakeCache.txt");

  if(!input)
  {
    input = ifstream_open_cstr("CMakeLists.txt");
  }

  if(!input)
  {
    abort();
  }

  data = vector_new(unsigned char);
  vector_resize(data, 500);
  ifstream_read(input, data);
  printf("Read: %i\n", (int)vector_size(data));

  ifstream_close(input);
  vector_delete(data);

  return 0;
}
