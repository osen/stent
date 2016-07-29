#include "StentMongoose.h"

#include "mongoose.h"

struct MgManager
{
  struct mg_mgr mgr;
  REF(Object) userData;
  ARRAY(MgConnection) connections;
};

struct MgConnection
{
  struct mg_connection *nc;
  REF(MgManager) mgr;
  REF(MgConnection) self;
  void (*ev_handler)(REF(MgConnection), int, REF(Object));
};

void MgManagerFinalize(REF(MgManager) ctx)
{
  mg_mgr_free(&GET(ctx)->mgr);
  TRYFREE(GET(ctx)->connections);
}

REF(MgManager) MgManagerInit(REF(Object) userData)
{
  REF(MgManager) rtn = {};

  rtn = CALLOC(MgManager);

  GET(rtn)->connections = ARRAY_ALLOC(MgConnection);
  ARRAY_AUTOFREE(GET(rtn)->connections);
  GET(rtn)->userData = userData;
  mg_mgr_init(&GET(rtn)->mgr, GET(rtn));
  FINALIZER(rtn, MgManagerFinalize);

  return rtn;
}

static void _MgEvHandler(struct mg_connection *nc, int ev, void *p)
{
  REF(MgConnection) listenNc = {};
  struct MgConnection *conn = nc->user_data;
  listenNc = conn->self;

  // TODO: Verify nc, create object, generate p
  GET(listenNc)->ev_handler(listenNc, ev, NULLREF(Object));
}

void MgConnectionFinalize(REF(MgConnection) ctx)
{
  TRYFREE(GET(ctx)->mgr);
}

REF(MgConnection) MgBind(REF(MgManager) mgr, char *interface,
  void (*ev_handler)(REF(MgConnection), int, REF(Object)))
{
  REF(MgConnection) rtn = {};

  rtn = CALLOC(MgConnection);
  FINALIZER(rtn, MgConnectionFinalize);
  GET(rtn)->mgr = mgr;
  GET(rtn)->nc = mg_bind(&GET(mgr)->mgr, interface, _MgEvHandler);
  GET(rtn)->self = rtn;
  GET(rtn)->nc->user_data = GET(rtn);
  GET(rtn)->ev_handler = ev_handler;

  if(GET(rtn)->nc == NULL)
  {
    THROW(0, "Failed to bind");
  }

  ARRAY_ADD(GET(mgr)->connections, rtn);

  return rtn;
}

void MgSetProtocolHttpWebsocket(REF(MgConnection) nc)
{
  mg_set_protocol_http_websocket(GET(nc)->nc);
}

void MgManagerPoll(REF(MgManager) mgr, int timeout)
{
  mg_mgr_poll(&GET(mgr)->mgr, timeout);
}

