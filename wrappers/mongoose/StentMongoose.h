#include <stent.h>

REFDEF(MgManager);
REFDEF(MgConnection);

REF(MgManager) MgManagerInit(REF(Object) userData);

REF(MgConnection) MgBind(REF(MgManager) mgr, char *interface,
  void (*ev_handler)(REF(MgConnection), int, REF(Object)));
