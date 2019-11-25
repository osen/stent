#include <stdlib.h>

int main(int argc, char *argv[])
{
  int rc = 0;

  if(argc < 2)
  {
    abort();
  }

  rc = system(argv[1]);

  if(rc == 0)
  {
    rc = 1;
  }
  else
  {
    rc = 0;
  }

  return rc;
}
