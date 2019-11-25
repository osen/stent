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
  size_t i = 0;

  emps = vector_new(struct Employee);

  for(i = 0; i < 1000; i++)
  {
    emp.id = i;
    vector_push_back(emps, emp);
  }

  vector_erase(emps, 500, 100);

  if(vector_size(emps) != 900)
  {
    abort();
  }

  if(vector_at(emps, 899).id != 999)
  {
    abort();
  }

  vector_delete(emps);

  return 0;
}
