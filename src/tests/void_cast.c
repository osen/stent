#define STENT_IMPLEMENTATION
#include <stent.h>

struct Employee
{
  int id;
};

int main()
{
  ref(Employee) emp = NULL;
  refvoid vemp = NULL;

  emp = allocate(Employee);
  vemp = void_cast(emp);
  emp = cast(Employee, vemp);

  _(emp).id = 9;

  release(emp);

  return 0;
}
