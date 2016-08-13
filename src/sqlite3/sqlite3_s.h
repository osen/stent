#include "sqlite3.h"

#include <stent.h>

refdef(sqlite3_s);
refdef(sqlite3_stmt_s);

ref(sqlite3_s) sqlite3_open_s(char *path);
ref(sqlite3_stmt_s) sqlite3_prepare_v2_s(ref(sqlite3_s), char *);
int sqlite3_step_s(ref(sqlite3_stmt_s));
void sqlite3_bind_text_s(ref(sqlite3_stmt_s), int, const char *);
void sqlite3_bind_int64_s(ref(sqlite3_stmt_s), int, sqlite3_int64);
void sqlite3_bind_int_s(ref(sqlite3_stmt_s), int, int);
int sqlite3_column_int_s(ref(sqlite3_stmt_s), int);
ref(String) sqlite3_column_text_s(ref(sqlite3_stmt_s), int);
