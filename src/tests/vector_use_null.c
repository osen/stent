#define STENT_IMPLEMENTATION
#include <stent.h>

struct Employee
{
  int id;
};

int main()
{
  vector(struct Employee) emps = NULL;
  struct Employee emp = {0};

  emp.id = 1;
  vector_push_back(emps, emp);

  return 0;
}
