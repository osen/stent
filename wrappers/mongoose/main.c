#include "StentMongoose.h"

#include <stent.h>

void ev_handler(REF(MgConnection) nc, int ev, REF(Object) evData)
{
  printf("Event incoming: %i\n", ev);
}

void safe_main(REF(Object) userData)
{
  REF(MgManager) mgr = {};
  REF(MgConnection) nc = {};

  mgr = MgManagerInit(NULLREF(Object));
  nc = MgBind(mgr, "8080", ev_handler);

  MgSetProtocolHttpWebsocket(nc);

  while(1)
  {
    MgManagerPoll(mgr, 1000);
  }

  FREE(mgr);
  //FREE(nc);
}

int main(int argc, char *argv[])
{
  int rtn = 0;
  REF(Exception) ex = {};
  REF(Object) ud = {};

  ud = CALLOC(Object);

  ex = TRY(safe_main, ud);

  if(TRYGET(ex) != NULL)
  {
    printf("Exception: %s\n", GET(ex)->message);
    rtn = 1;
  }

  FREE(ud);

  StentStats();
  StentCleanup();

  return rtn;
}
