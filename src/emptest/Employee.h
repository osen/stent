#include <stent.h>

refdef(Employee);
ref(Department);

ref(Employee) EmployeeCreate(ref(Department) dept);
int EmployeeId(ref(Employee) ctx);
int EmployeeLevel(ref(Employee) ctx);
