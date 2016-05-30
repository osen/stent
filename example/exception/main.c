#include <stent.h>

REFDEF(Test);
REFDEF(InvalidTest);

struct Test
{
  int someData;
};

struct InvalidTest
{
  char b;
};

void safe_main(REF(Object) userData)
{
  //REF(InvalidTest) test = CAST(InvalidTest, userData);
  REF(Test) test = CAST(Test, userData);

  printf("Result: %p\n", GET(test));

  THROW(0, "Something exceptional has happened");
}

int main(int argc, char *argv[])
{
  REF(Exception) exception = {};
  REF(Test) test = {};

  test = CALLOC(Test);

  //THROW(0, "Something exceptional has happened");
  safe_main(CAST(Object, test));
  //exception = TRY(safe_main, CAST(Object, test));

  if(TRYGET(exception) != NULL)
  {
    printf("There was an exception\n");
    FREE(exception);
  }

  FREE(test);

  RefStats();
  RefCleanup();

  return 0;
}
