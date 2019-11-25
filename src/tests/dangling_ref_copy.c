#define STENT_IMPLEMENTATION
#include <stent.h>

ref(Employee) copy;

struct Employee
{
  int id;
};

int main()
{
  ref(Employee) emp = NULL;

  emp = allocate(Employee);
  copy = emp;

  release(emp);

  _(copy).id = 9;

  return 0;
}
