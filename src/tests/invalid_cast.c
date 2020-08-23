#define STENT_IMPLEMENTATION
#include <stent.h>

struct Employee
{
  int id;
};

struct Manager
{
  int id;
};

int main()
{
  ref(Employee) emp = NULL;
  ref(Manager) mgr = NULL;

  emp = allocate(Employee);
  mgr = cast(Manager, emp);

  _(mgr).id = 9;

  release(emp);

  return 0;
}
