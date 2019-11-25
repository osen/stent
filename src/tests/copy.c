#define STENT_IMPLEMENTATION
#include <stent.h>

struct Employee
{
  int id;
  int salary;
  ref(Employee) manager;
};

int main()
{
  ref(Employee) emp = NULL;
  struct Employee e = {0};

  e.id = 9;
  e.salary = 10;
  e.manager = NULL;

  emp = allocate(Employee);
  _(emp) = e;

  _(emp).id = 9;

  release(emp);

  return 0;
}
