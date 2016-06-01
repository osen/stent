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

  object = CAST(Object, test);
  test = CAST(Test, test);

  printf("Some Data: %i\n", GET(test)->someData);

  FREE(test);

  StentStats();
  StentCleanup();

  return 0;
}
