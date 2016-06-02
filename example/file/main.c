#include <stentext.h>

void safe_main(REF(Object) userData)
{
  REF(InputFile) ifile = {};
  REF(String) lines = {};

  FREE(userData);
  ifile = InputFileOpen("build.sh");
  lines = StringEmpty();

  while(!InputFileEof(ifile))
  {
    InputFileReadLine(ifile, lines);
    StringAddChar(lines, '\n');
  }

  printf("Contents Length: %lu\n", (unsigned long)StringLength(lines));
  FREE(lines);
  FREE(ifile);
}

int main(int argc, char *argv[])
{
  REF(Exception) exception = {};
  REF(Object) userData = {};

  userData = CALLOC(Object);
  exception = TRY(safe_main, userData);

  if(TRYGET(exception) != NULL)
  {
    printf("%s\n", GET(exception)->message);
    FREE(exception);
  }

  StentStats();
  StentCleanup();

  return 0;
}
