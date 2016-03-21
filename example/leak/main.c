#include <stent.h>

DECLARE(Test);

struct Test
{
  int someData;
};

int main(int argc, char *argv[])
{
  REF(Test) leaking = {};

  leaking = CALLOC(Test);
  GET(leaking)->someData = 9;

  RefStats();
  RefCleanup();

  return 0;
}
