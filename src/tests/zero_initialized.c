#define STENT_IMPLEMENTATION
#include <stent.h>

struct Employee
{
  int id;
};

int main()
{
  ref(Employee) emp = NULL;

  emp = allocate(Employee);

  if(_(emp).id != 0)
  {
    abort();
  }

  release(emp);

  return 0;
}
