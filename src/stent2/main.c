#include <stent.h>

#include <stdio.h>

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

/*
ref(FILE) FileTest(ref(sstream) s)
{
  FILE *f = NULL;
  ref(FILE) rtn = NULL;

  f = fopen(sstream_cstr(s), "r");
  rtn = sattach(FILE, f);

  return rtn;
}
*/

int main()
{
  ref(struct Employee) emp = NULL;
  /* ref(int) pint = NULL; */
  vector(int) ages = NULL;
  vector(ref(struct Employee)) emps = NULL;
  vector(struct Employee) semps = NULL;
  struct Employee semp = {0};
  int i = 0;

  emps = vector_new(ref(struct Employee));

  for(i = 0; i < 5; i++)
  {
    emp = EmployeeCreate();
    vector_push_back(emps, emp);
  }

  printf("Employee Vector size %i\n", (int)vector_size(emps));

  for(i = 0; i < vector_size(emps); i++)
  {
    /* EmployeeDestroy(vector_at(emps, i)); */
    /* sfree(vector_at(emps, i)); */
    EmployeeInfo(vector_at(emps, i));
    /* _(vector_at(emps, i))->age = 8; */
  }

  vector_delete(emps);
  /* vector_delete(pint); */

  semps = vector_new(struct Employee);
  vector_push_back(semps, semp);
  EmployeeInfoSt(&semp);
  /* vector_at(semps, 0); */
  vector_delete(semps);

  ages = vector_new(int);
  vector_push_back(ages, 9);
  vector_push_back(ages, 99);
  printf("Int size %i %i\n", (int)vector_size(ages), vector_at(ages, 0));
  vector_delete(ages);

  return 0;
}
