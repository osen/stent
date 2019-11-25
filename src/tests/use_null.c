#define STENT_IMPLEMENTATION
#include <stent.h>

struct Employee
{
  int id;
};

int main()
{
  ref(Employee) emp = NULL;

  _(emp).id = 9;

  return 0;
}
