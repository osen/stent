#include <stent.h>

#include <stdio.h>

struct Employee
{
  int age;
};

ref(struct Employee) EmployeeCreate()
{
  ref(struct Employee) rtn;

  rtn = palloc(struct Employee);
  _(rtn)->age = 28;

  return rtn;
}

void EmployeeDestroy(ref(struct Employee) ctx)
{
  pfree(ctx);
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
  rtn = pattach(FILE, f);

  return rtn;
}
*/

int main()
{
  ref(struct Employee) emp = NULL;
  struct Employee semp = {0};
  int i = 0;

  for(i = 0; i < 1024; i++)
  {
    emp = EmployeeCreate();
    EmployeeInfo(emp);
    EmployeeDestroy(emp);
  }

  EmployeeInfoSt(&semp);

  return 0;
}
