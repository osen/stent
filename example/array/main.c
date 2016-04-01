#include <stent.h>
#include <stentext.h>

//REF(Employee);
//ARRAY(Employee);

//void print_employee(REF(Employee) employee);
//void print_employees(ARRAY(Employee) employees);

REFDEF(Employee);

struct Employee
{
  int id;
};

void print_employees(ARRAY(Employee) employees)
{

}

int main(int argc, char *argv[])
{
  REF(Employee) employee = {};
  ARRAY(Employee) employees = {};

  employee = CALLOC(Employee);
  GET(employee)->id = 9;

  employees = ARRAY_ALLOC(Employee);
  //ARRAY_ADD(CALLOC(Employee));

  print_employees(employees);

  FREE(employee);

  RefStats();
  RefCleanup();

  return 0;
}
