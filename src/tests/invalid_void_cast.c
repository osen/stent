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
  refvoid vemp = NULL;
  ref(Manager) mgr = NULL;

  emp = allocate(Employee);
  vemp = void_cast(emp);

  /*
   * Error: Attempt to cast Employee into Manager.
   */
  mgr = cast(Manager, vemp);

  release(emp);

  return 0;
}
