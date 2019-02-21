#include <stent.h>

#include <stdio.h>

/*
#define EMP_VECTOR_TEST
#define INT_VECTOR_TEST
#define EMP_REF_TEST
#define INT_REF_TEST
#define SSTREAM_TEST
#define FILE_TEST
*/
#define EMP_VECTOR_TEST
#define INT_VECTOR_TEST

struct Employee
{
  int age;
};

ref(struct Employee) EmployeeCreate()
{
  ref(struct Employee) rtn;

  rtn = salloc(struct Employee);
  _(rtn)->age = 28;

  return rtn;
}

void EmployeeDestroy(ref(struct Employee) ctx)
{
  sfree(ctx);
}

void EmployeeInfo(ref(struct Employee) ctx)
{
  printf("Age: %i\n", _(ctx)->age);
}

void EmployeeInfoSt(struct Employee *ctx)
{
  printf("Age: %i\n", ctx->age);
}

#ifdef FILE_TEST

char *fgets_s(char *str, int size, ref(FILE) f)
{
  return fgets(str, size, _(f));
}

ref(FILE) fopen_s(const char *path, const char *mode)
{
  ref(FILE) rtn = NULL;

  rtn = salloc_placement(FILE, fopen(path, mode));
  if(!rtn) return NULL;

  return rtn;
}
#endif

int main()
{
#ifdef EMP_REF_TEST
{
  ref(struct Employee) emp = NULL;

  emp = EmployeeCreate();
  EmployeeInfo(emp);
  EmployeeDestroy(emp);
}
#endif

#ifdef INT_REF_TEST
{
  ref(int) i = NULL;

  i = salloc(int);
  *_(i) = 9;
  ++*_(i);
  (*_(i)) ++;
  ++(*_(i));
  printf("Integer ref is %i\n", *_(i));
  sfree(i);
}
#endif

#ifdef INT_VECTOR_TEST
{
  vector(int) ages = NULL;

  ages = vector_new(int);
  vector_push_back(ages, 9);
  vector_push_back(ages, 99);

  printf("Int Vector size %i [0] %i\n",
    (int)vector_size(ages), vector_at(ages, 0));

  vector_delete(ages);
}
#endif

#ifdef EMP_VECTOR_TEST
{
  vector(ref(struct Employee)) emps = NULL;
  int i = 0;

  emps = vector_new(ref(struct Employee));
  printf("Employee Vector size %i\n", (int)vector_size(emps));

  for(i = 0; i < 5; i++)
  {
    vector_push_back(emps, EmployeeCreate());
  }

  printf("Employee Vector size %i\n", (int)vector_size(emps));

  for(i = 0; i < vector_size(emps); i++)
  {
    EmployeeInfo(vector_at(emps, i));
/*
    _(vector_at(emps, i))->age = 8;
*/
  }

  vector_delete(emps);
}
#endif

#ifdef SSTREAM_TEST
  sstream ss = NULL;

  ss = sstream_new();
  sstream_str(ss, "Hello World");

  sstream_at(ss, 0);
  printf("StringStream: %s\n", sstream_cstr(ss));

  sstream_delete(ss);
#endif

#ifdef FILE_TEST
  ref(FILE) f = NULL;
  char text[128];

  f = fopen_s("a.out", "r");
  /* printf("[%s]\n", fgets_s(text, 128, f)); */
  /* sfree(f); */
  fgets_s(text, 128, f);

#endif

  return 0;
}
