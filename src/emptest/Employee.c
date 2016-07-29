#include "Employee.h"
#include "Department.h"

#include <stdlib.h>

object(Role)
{
  int type;
};
refdef(Role);

object(Employee)
{
  int id;
  int level;
  ref(Department) dept;
  ref(Role) role;
};

static void RoleFinalize(ref(Role) ctx)
{
  printf("~Role\n");
}

ref(Role) RoleCreate(int type)
{
  ref(Role) rtn = {0};

  rtn = create(Role);
  get(rtn)->type = type;

  finalizer(rtn, RoleFinalize);

  return rtn;
}

static void EmployeeFinalize(ref(Employee) ctx)
{
  printf("~Employee\n");
}

ref(Employee) EmployeeCreate(ref(Department) dept)
{
  ref(Employee) rtn = {0};

  rtn = create(Employee);
  strong(dept, rtn);
  //strong(rtn, dept);

  finalizer(rtn, EmployeeFinalize);
  get(rtn)->id = ((float)rand() / (float)RAND_MAX) * 100;
  get(rtn)->dept = dept;
  get(rtn)->role = RoleCreate(7);
  strong(rtn, get(rtn)->role);

  return rtn;
}

int EmployeeId(ref(Employee) ctx)
{
  return get(ctx)->id;
}

int EmployeeLevel(ref(Employee) ctx)
{
  return get(ctx)->level;
}
