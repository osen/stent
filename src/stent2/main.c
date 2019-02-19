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
  vector(int) ages = NULL;
  vector(ref(struct Employee)) emps = NULL;
  struct Employee semp = {0};
  int i = 0;

  for(i = 0; i < 5; i++)
  {
    emp = EmployeeCreate();
    EmployeeInfo(emp);
    /* EmployeeDestroy(emp); */
  }

  EmployeeInfoSt(&semp);

  ages = vector_new(int);

  vector_push_back(ages, 9);
  vector_push_back(ages, 99);

  printf("Vector size %i\n", (int)vector_size(ages));

  vector_delete(ages);

  emps = vector_new(ref(struct Employee));
  vector_push_back(emps, emp);
  /*vector_delete(emps);*/
  printf("Vector size %i\n", (int)vector_size(emps));
  /* EmployeeInfo(emps[0][0][0]); */

  return 0;
}
