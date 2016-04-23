#include <stent.h>

REFDEF(Test);

struct Test
{
  int someData;
};

void safe_main(REF(Object) userData)
{
  THROW(0, "Something exceptional has happened");
}

int main(int argc, char *argv[])
{
  REF(Exception) exception = {};
  REF(Test) test = {};

  test = CALLOC(Test);

  //exception = TRY(safe_main, CAST(Object, test));
  THROW(0, "Something exceptional has happened");

  FREE(exception);
  FREE(test);

  RefStats();
  RefCleanup();

  return 0;
}
