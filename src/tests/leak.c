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

  return 0;
}
