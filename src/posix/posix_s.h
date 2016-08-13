#include <stent.h>

refdef(FILE_s);

ref(FILE_s) popen_s(const char *command, const char *mode);
void pclose_s(ref(FILE_s) ctx);
size_t _fread_s(ref(CharArray) str, ref(FILE_s) stream);
