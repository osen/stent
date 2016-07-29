#include <stent.h>

REFDEF(First);
REFDEF(Second);
REFDEF(Third);
REFDEF(UserData);

struct UserData
{
  int a;
};

struct First
{
  int a;
};

struct Second
{
  char b;
};

struct Third
{
  float c;
};

void FirstFinalizer(REF(First) ctx)
{
  printf("~First\n");
}

void SecondFinalizer(REF(Second) ctx)
{
  printf("~Second\n");
}

void ThirdFinalizer(REF(Third) ctx)
{
  printf("~Third\n");
}

void safe_main(REF(UserData) userData)
{
  REF(First) first = {};
  REF(Second) second = {};
  REF(Third) third = {};

  first = CALLOC(First);
  FINALIZER(first, FirstFinalizer);

  FREE(userData);

  second = CALLOC(Second);
  FINALIZER(second, SecondFinalizer);

  third = CALLOC(Third);
  FINALIZER(third, ThirdFinalizer);

  THROW(0, "Something exceptional has happened");

  FREE(third);
  FREE(second);
  FREE(first);
}

int main(int argc, char *argv[])
{
  REF(Exception) exception = {};
  REF(UserData) userData = {};

  userData = CALLOC(UserData);
  exception = TRY(safe_main, userData);

  if(TRYGET(exception) != NULL)
  {
    printf("%s\n", GET(exception)->message);
    FREE(exception);
  }

  //printf("User Data: %p\n", GET(userData));
  //FREE(userData);

  StentStats();
  StentCleanup();

  return 0;
}
