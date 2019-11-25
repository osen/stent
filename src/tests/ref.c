#define STENT_IMPLEMENTATION
#include <stent.h>

struct Employee
{
  int id;
  int wage;
};

ref(Employee) EmployeeCreate()
{
  ref(Employee) rtn = NULL;

  rtn = allocate(Employee);
  _(rtn).id = 9;

  return rtn;
}

void EmployeeDestroy(ref(Employee) ctx)
{
  release(ctx);
}

void EmployeeAddWage(ref(Employee) ctx, int amount)
{
  _(ctx).wage += amount;
}

int main()
{
  ref(Employee) emp = NULL;

  emp = EmployeeCreate();
  EmployeeAddWage(emp, 10);
  EmployeeDestroy(emp);

  return 0;
}
