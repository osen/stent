#include <stent.h>

REFDEF(Test);

struct Test
{
  int someData;
};

int main(int argc, char *argv[])
{
  REF(Test) leaking = {};

  leaking = CALLOC(Test);
  GET(leaking)->someData = 9;

  //FREE(leaking);

  RefStats();
  RefCleanup();

  return 0;
}
