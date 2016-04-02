#include <stent.h>
#include <stentext.h>

#include <stdio.h>

// Forward declares using stent
REF(Employee);
ARRAY(Employee);

// Forward declaration of functions
void print_employee(REF(Employee) employee);
void print_employees(ARRAY(Employee) employees);

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

void print_employees(ARRAY(Employee) employees)
{
  int i = 0;

  for(i = 0; i < ARRAY_SIZE(employees); i++)
  {
    print_employee(ARRAY_AT(employees, i));
  }
}

int main(int argc, char *argv[])
{
  REF(Employee) employee = {};
  ARRAY(Employee) employees = {};

  employee = CALLOC(Employee);
  GET(employee)->id = 9;

  employees = ARRAY_ALLOC(Employee);
  ARRAY_ADD(employees, employee);
  ARRAY_ADD(employees, CALLOC(Employee));

  print_employees(employees);

  // Test out of bounds abort()
  //print_employee(ARRAY_AT(employees, 100));

  FREE(employee);
  FREE(ARRAY_AT(employees, 1));
  ARRAY_FREE(employees);

  // Double free? Should be fine
  FREE(employee);
  ARRAY_FREE(employees);

  RefStats();
  RefCleanup();

  return 0;
}
