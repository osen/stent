#include "sqlite3_s.h"

#include "sqlite3.h"

object(sqlite3_s)
{
  sqlite3 *raw;
};

object(sqlite3_stmt_s)
{
  sqlite3_stmt *raw;
};

static void sqlite3_s_finalize(ref(sqlite3_s) ctx)
{
  sqlite3_close(get(ctx)->raw);
}

ref(sqlite3_s) sqlite3_open_s(char *path)
{
  ref(sqlite3_s) rtn = {0};
  int rc = 0;

  rtn = create(sqlite3_s);

  rc = sqlite3_open(path, &get(rtn)->raw);
  finalizer(rtn, sqlite3_s_finalize);

  if(rc != SQLITE_OK)
  {
    throw(0, sqlite3_errmsg(get(rtn)->raw));
  }

  return rtn;
}

static void sqlite3_stmt_s_finalize(ref(sqlite3_stmt_s) ctx)
{
  sqlite3_finalize(get(ctx)->raw);
}

ref(sqlite3_stmt_s) sqlite3_prepare_v2_s(ref(sqlite3_s) db, char *sql)
{
  ref(sqlite3_stmt_s) rtn = {0};
  int rc = 0;

  rtn = create(sqlite3_stmt_s);
  strong(db, rtn);

  rc = sqlite3_prepare_v2(get(db)->raw, sql, -1, &get(rtn)->raw, 0);

  if(rc != SQLITE_OK)
  {
    throw(0, sqlite3_errmsg(get(db)->raw));
  }

  finalizer(rtn, sqlite3_stmt_s_finalize);

  return rtn;
}

int sqlite3_step_s(ref(sqlite3_stmt_s) stmt)
{
  int rtn = 0;

  rtn = sqlite3_step(get(stmt)->raw);

  return rtn;
}

void sqlite3_bind_int64_s(ref(sqlite3_stmt_s) stmt, int n, sqlite3_int64 value)
{
  sqlite3_bind_int64(get(stmt)->raw, n, value);
}

void sqlite3_bind_int_s(ref(sqlite3_stmt_s) stmt, int n, int value)
{
  sqlite3_bind_int(get(stmt)->raw, n, value);
}

void sqlite3_bind_text_s(ref(sqlite3_stmt_s) stmt, int n, const char *value)
{
  sqlite3_bind_text(get(stmt)->raw, n, value, -1, SQLITE_TRANSIENT);
}

int sqlite3_column_int_s(ref(sqlite3_stmt_s) ctx, int n)
{
  return sqlite3_column_int(get(ctx)->raw, n);
}

ref(String) sqlite3_column_text_s(ref(sqlite3_stmt_s) ctx, int n)
{
  ref(String) rtn = {0};

  rtn = StringFromCStr(sqlite3_column_text(get(ctx)->raw, n));
  strong(ctx, rtn);

  return rtn;
}
