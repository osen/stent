#include <stent.h>
#include <stentext.h>

#include <stdio.h>

// Forward declares using stent
REF(Employee);
REF_ARRAY(Employee);

// Forward declaration of functions
void print_employee(REF(Employee) employee);
void print_employees(REF_ARRAY(Employee) employees);

// Prepare type for use with stent. Similar to typedef
REFDEF(Employee);

struct Employee
{
  int id;
};

void print_employee(REF(Employee) employee)
{
  printf("Employee ID: %i\n", GET(employee)->id);
}

void print_employees(REF_ARRAY(Employee) employees)
{
  int i = 0;

  for(i = 0; i < REF_ARRAY_SIZE(employees); i++)
  {
    print_employee(REF_ARRAY_AT(employees, i));
  }
}

int main(int argc, char *argv[])
{
  REF(Employee) employee = {};
  REF_ARRAY(Employee) employees = {};

  employee = CALLOC(Employee);
  GET(employee)->id = 9;

  employees = REF_ARRAY_ALLOC(Employee);
  REF_ARRAY_ADD(employees, employee);
  REF_ARRAY_ADD(employees, CALLOC(Employee));
  REF_ARRAY_ADD(employees, employee);
  REF_ARRAY_ADD(employees, employee);
  REF_ARRAY_ADD(employees, employee);
  REF_ARRAY_ADD(employees, employee);
  REF_ARRAY_ADD(employees, employee);
  REF_ARRAY_ADD(employees, employee);
  REF_ARRAY_ADD(employees, employee);
  REF_ARRAY_ADD(employees, employee);

  REF_ARRAY_REMOVEAT(employees, REF_ARRAY_SIZE(employees) - 1);
  REF_ARRAY_REMOVEAT(employees, 2);
  REF_ARRAY_REMOVEAT(employees, 2);
  REF_ARRAY_REMOVEAT(employees, 2);
  REF_ARRAY_REMOVEAT(employees, 2);
  REF_ARRAY_REMOVEAT(employees, 2);
  REF_ARRAY_REMOVEAT(employees, 2);
  REF_ARRAY_REMOVEAT(employees, 2);

  print_employees(employees);

  // Test out of bounds abort()
  //print_employee(REF_ARRAY_AT(employees, 100));

  FREE(employee);
  FREE(REF_ARRAY_AT(employees, 1));
  REF_ARRAY_FREE(employees);

  // Double free? Should be fine
  FREE(employee);
  REF_ARRAY_FREE(employees);

  RefStats();
  RefCleanup();

  return 0;
}
