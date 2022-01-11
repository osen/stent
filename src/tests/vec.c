#define VEC_IMPLEMENTATION
#include <vec.h>

struct Employee
{
  int id;
};

int main()
{
  vec(struct Employee) emps = NULL;
  vec(int) ids = NULL;
  size_t i = 0;

  ids = vec_new(int);
  emps = vec_new(struct Employee);

  for(i = 0; i < 1000; i++)
  {
    struct Employee emp = {0};
    emp.id = i;
    vec_push_back(emps, emp);
    vec_push_back(ids, i);
  }

  vec_erase(emps, 500, 100);
  vec_erase(ids, 500, 100);

  if(vec_size(emps) != 900)
  {
    abort();
  }

  if(vec_size(ids) != 900)
  {
    abort();
  }

  if(vec_at(emps, 899).id != 999)
  {
    abort();
  }

  if(vec_at(ids, 899) != 999)
  {
    abort();
  }

  vec_delete(emps);
  vec_delete(ids);

  return 0;
}
