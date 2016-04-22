#include <stent.h>

REFDEF(Test);

struct Test
{
  int someData;
};

int main(int argc, char *argv[])
{
  REF(Test) test = {};
  REF(Object) object = {};

  test = CALLOC(Test);
  GET(test)->someData = 9;

  //object = CAST(Object, test);

  FREE(test);

  RefStats();
  RefCleanup();

  return 0;
}
