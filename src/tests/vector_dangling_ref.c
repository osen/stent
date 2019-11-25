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

  emps = vector_new(struct Employee);
  vector_push_back(emps, emp);
  vector_delete(emps);
  vector_push_back(emps, emp);

  return 0;
}
