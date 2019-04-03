#include <stent.h>

#include <stdio.h>

/*
#define EMP_VECTOR_TEST
#define INT_VECTOR_TEST
#define FLOAT_VECTOR_TEST
#define EMP_REF_TEST
#define INT_REF_TEST
#define SSTREAM_TEST
#define FILE_TEST
*/
#define FLOAT_VECTOR_TEST

struct Employee
{
  int age;
  ref(vector(unsigned char)) buffer;
};

ref(struct Employee) EmployeeCreate()
{
  ref(struct Employee) rtn;

  rtn = salloc(struct Employee);
  _(rtn)->age = 28;
  _(rtn)->buffer = vector_new(unsigned char);
  vector_push_back(_(rtn)->buffer, 'a');
  vector_push_back(_(rtn)->buffer, 'b');

  return rtn;
}

void EmployeeDestroy(ref(struct Employee) ctx)
{
  vector_delete(_(ctx)->buffer);
  sfree(ctx);
}

void EmployeeInfo(ref(struct Employee) ctx)
{
  printf("Age: %i\n", _(ctx)->age);
  printf("Buff: %i\n", (int)vector_at(_(ctx)->buffer, 0));
  vector_at(_(ctx)->buffer, 0) = '8';
  vector_at(_(ctx)->buffer, 1) = '8';
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

#ifdef FLOAT_VECTOR_TEST
{
  ref(vector(float)) values = NULL;
  ref(vector(float)) missing = NULL;
  int i = 0;

  values = vector_new(float);
  vector_push_back(values, 0);
  vector_push_back(values, 1);
  vector_push_back(values, 2);
  vector_push_back(values, 3);
  vector_push_back(values, 4);
  vector_push_back(values, 5);
  vector_push_back(values, 6);
  vector_push_back(values, 7);
  vector_push_back(values, 8);
  vector_push_back(values, 9);

  vector_erase(values, 3, 3);

  missing = vector_new(float);
  vector_push_back(missing, 3);
  vector_push_back(missing, 4);
  vector_push_back(missing, 5);

  vector_insert(values, 3, missing, 0, vector_size(missing));
  vector_delete(missing);

  printf("?[0123456789]\n");
  printf("![");
  for(i = 0; i < vector_size(values); i++)
  {
    printf("%i", (int)vector_at(values, i));
  }
  printf("]\n");

  vector_delete(values);
}
#endif

#ifdef INT_VECTOR_TEST
{
  ref(vector(int)) ages = NULL;
  int i = 0;

  ages = vector_new(int);

  printf("Int Vector size %i\n", (int)vector_size(ages));

  vector_push_back(ages, 0);
  vector_push_back(ages, 1);
  vector_push_back(ages, 2);
  vector_push_back(ages, 3);
  vector_push_back(ages, 4);
  vector_push_back(ages, 5);
  vector_push_back(ages, 6);
  vector_push_back(ages, 7);
  vector_push_back(ages, 8);
  vector_push_back(ages, 9);

  printf("Int Vector size %i [0] %i\n",
    (int)vector_size(ages), vector_at(ages, 0));

  printf("?[0123456789]\n");
  printf("![");
  for(i = 0; i < vector_size(ages); i++)
  {
    printf("%i", vector_at(ages, i));
  }
  printf("]\n");

  printf("?[0189]\n");
  vector_erase(ages, 2, 6);

  printf("![");
  for(i = 0; i < vector_size(ages); i++)
  {
    printf("%i", vector_at(ages, i));
  }
  printf("]\n");

  printf("?[189]\n");
  vector_erase(ages, 0, 1);
  printf("![");
  for(i = 0; i < vector_size(ages); i++)
  {
    printf("%i", vector_at(ages, i));
  }
  printf("]\n");

  printf("?[18]\n");
  vector_erase(ages, 2, 1);
  printf("![");
  for(i = 0; i < vector_size(ages); i++)
  {
    printf("%i", vector_at(ages, i));
  }
  printf("]\n");

  printf("?[]\n");
/*
  vector_erase(ages, 1, 1);
  vector_erase(ages, 0, 1);
*/
  vector_erase(ages, 0, 2);
  printf("![");
  for(i = 0; i < vector_size(ages); i++)
  {
    printf("%i", vector_at(ages, i));
  }
  printf("]\n");

  vector_delete(ages);
}
#endif

#ifdef EMP_VECTOR_TEST
{
  ref(vector(ref(struct Employee))) emps = NULL;
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
{
  sstream ss = NULL;

  ss = sstream_new();
  sstream_str(ss, "Hello World");

  sstream_at(ss, 0);
  printf("StringStream: %s\n", sstream_cstr(ss));

  sstream_delete(ss);
}
#endif

#ifdef FILE_TEST
{
  ref(FILE) f = NULL;
  char text[128] = {0};

  f = fopen_s("a.out", "r");
  /* printf("[%s]\n", fgets_s(text, 128, f)); */
  /* sfree(f); */
  fgets_s(text, 128, f);
}
#endif

  return 0;
}
