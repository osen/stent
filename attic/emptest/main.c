#include "Employee.h"
#include "Department.h"

#include <stent.h>

#include <stdio.h>

ref(Employee) jeff;

void safe_main(ref(Object) userData)
{
  ref(Department) department = {0};
  ref(Employee) employee = {0};

  printf("Stent Employee Test\n");

  department = DepartmentCreate();
  employee = EmployeeCreate(department);

  printf("Employee ID: %i\n", EmployeeId(employee));
  printf("Employee Level: %i\n", EmployeeLevel(employee));

  jeff = employee;

/*
  destroy(employee);

  printf("Employee ID: %i\n", EmployeeId(employee));
  printf("Employee Level: %i\n", EmployeeLevel(employee));
*/

  throw(0, "Example of an exception to try for leaks");

  printf("Jeff is %p\n", get(jeff));

  destroy(department);

  if(!valid(jeff))
  {
    printf("Jeff is NULL\n");
  }
}

int main(int argc, char *argv[])
{
  ref(Object) obj = {0};
  ref(Exception) ex = {0};
  int rtn = 0;

  obj = create(Object);
  ex = try(safe_main, obj);
  destroy(obj);

  if(valid(ex))
  {
    printf("Exception: %s\n", ExceptionMessage(ex));
    rtn = 1;
    destroy(ex);
  }

  //printf("%i\n", get(jeff));

  stent_cleanup();

  return rtn;
}
