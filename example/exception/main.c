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

void safe_main(REF(Test) userData)
{
  REF(Test) tempTest = {};

  tempTest = CALLOC(Test);

  printf("Result: %p\n", GET(userData));

  THROW(0, "Something exceptional has happened");
}

int main(int argc, char *argv[])
{
  REF(Exception) exception = {};
  REF(Test) test = {};

  test = CALLOC(Test);

  //THROW(0, "Something exceptional has happened");
  //safe_main(CAST(Object, test));
  exception = TRY(safe_main, test);

  if(TRYGET(exception) != NULL)
  {
    printf("%s\n", GET(exception)->message);
    FREE(exception);
  }

  FREE(test);

  StentStats();
  StentCleanup();

  return 0;
}
