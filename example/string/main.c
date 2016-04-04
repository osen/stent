#include <stentext.h>

#include <stdio.h>

int main(int argc, char *argv[])
{
  REF(String) username = {};

  username = StringAllocCStr("kpedersen");

  printf("Username: %s\n", StringCStr(username));

  printf("String data (before free): %p\n", GET(username));
  StringFree(username);
  printf("String data (after free): %p\n", GET(username));

  RefStats();
  RefCleanup();

  return 0;
}
