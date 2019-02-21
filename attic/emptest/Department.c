#include "Department.h"

object(Department)
{
  ref(String) location;
};

static void DepartmentFinalize(ref(Department) ctx)
{
  printf("~Department\n");
}

ref(Department) DepartmentCreate()
{
  ref(Department) rtn = {0};

  rtn = create(Department);
  finalizer(rtn, DepartmentFinalize);
  get(rtn)->location = StringFromCStr("England");
  strong(rtn, get(rtn)->location);

  return rtn;
}
