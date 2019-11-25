#define STENT_IMPLEMENTATION
#include <stent.h>

ref(Employee) copy = NULL;

struct Employee
{
  int id;
};

int main()
{
  ref(Employee) emp = NULL;

  emp = allocate(Employee);
  copy = emp;
  _(copy).id = 9;
  release(emp);

  return 0;
}
