#include "mongoose_s.h"

object(mg_mgr_s)
{
  struct mg_mgr raw;
};

object(mg_connection_s)
{
  struct mg_connection *raw;
  ref(Object) user_data;
  void (*ev_handler)(ref(mg_connection_s), int, ref(Object));
  ref(mg_connection_s) self;
  array(mg_connection_s) connections;
};

object(http_message_s)
{
  struct http_message *raw;
  ref(String) uri;
  ref(String) query_string;
};

object(HandlerData)
{
  ref(mg_connection_s) nc;
  ref(mg_connection_s) handlerNc;
  int ev;
  ref(Object) ev_data;
};

refdef(HandlerData);

static void mg_mgr_s_finalizer(ref(mg_mgr_s) ctx)
{
  //printf("~mg_mgr_s\n");
  mg_mgr_free(&get(ctx)->raw);
}

static void mg_connection_s_finalizer(ref(mg_connection_s) ctx)
{
  //printf("~mg_connection_s\n");
}

ref(mg_mgr_s) mg_mgr_init_s(ref(Object) userData)
{
  ref(mg_mgr_s) rtn = {0};

  rtn = create(mg_mgr_s);
  mg_mgr_init(&get(rtn)->raw, NULL);
  finalizer(rtn, mg_mgr_s_finalizer);

  return rtn;
}

static void safe_ev_handler(ref(HandlerData) userData)
{
  ref(mg_connection_s) handlerNc = {0};

  handlerNc = get(userData)->handlerNc;
  get(handlerNc)->ev_handler(get(userData)->nc, get(userData)->ev, get(userData)->ev_data);
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
  ref(mg_connection_s) handlerNc = {0};
  ref(HandlerData) handlerData = {0};
  ref(Exception) ex = {0};

  handlerNc = ((struct mg_connection_s *)nc->user_data)->self;

  handlerData = create(HandlerData);
  get(handlerData)->handlerNc = handlerNc;
  get(handlerData)->ev = ev;

  if(ev == MG_EV_HTTP_REQUEST)
  {
    ref(http_message_s) ev_message = {0};
    ref(mg_connection_s) requestNc = {0};
    size_t i = 0;

    ev_message = create(http_message_s);
    strong(handlerData, ev_message);
    get(ev_message)->raw = ev_data;

    get(ev_message)->uri = StringCreate();
    strong(ev_message, get(ev_message)->uri);

    for(i = 0; i < get(ev_message)->raw->uri.len; i++)
    {
      StringAddChar(get(ev_message)->uri, get(ev_message)->raw->uri.p[i]);
    }

    get(ev_message)->query_string = StringCreate();
    strong(ev_message, get(ev_message)->query_string);

    for(i = 0; i < get(ev_message)->raw->query_string.len; i++)
    {
      StringAddChar(get(ev_message)->query_string, get(ev_message)->raw->query_string.p[i]);
    }

    get(handlerData)->ev_data = cast(Object, ev_message);

    requestNc = create(mg_connection_s);
    finalizer(requestNc, mg_connection_s_finalizer);
    strong(handlerData, requestNc);
    get(requestNc)->raw = nc;
    get(handlerData)->nc = requestNc;
  }

  ex = try(safe_ev_handler, handlerData);

  if(valid(ex))
  {
    printf("Exception: %s\n", ExceptionMessage(ex));
    destroy(ex);
  }

  destroy(handlerData);
}

ref(mg_connection_s) mg_bind_s(ref(mg_mgr_s) mgr, char *interface,
  void (*func)(ref(mg_connection_s), int, ref(Object)))
{
  ref(mg_connection_s) rtn = {0};
  struct mg_connection_s *prtn = NULL;

  rtn = create(mg_connection_s);
  strong(mgr, rtn);
  strong(rtn, mgr);
  finalizer(rtn, mg_connection_s_finalizer);
  get(rtn)->raw = mg_bind(&get(mgr)->raw, interface, ev_handler);

  if(!get(rtn)->raw)
  {
    throw(0, "Failed to bind on interface");
  }

  get(rtn)->raw->user_data = get(rtn);
  get(rtn)->ev_handler = func;

  get(rtn)->connections = array_create(mg_connection_s);
  strong(rtn, get(rtn)->connections);

  /* Hack for Plan 9 CC to avoid "Allocation out of bounds compiler bug". */
  /* get(rtn)->self = rtn; */
  prtn = get(rtn);
  prtn->self = rtn;

  return rtn;
}

void mg_set_protocol_http_websocket_s(ref(mg_connection_s) nc)
{
  mg_set_protocol_http_websocket(get(nc)->raw);
}

time_t mg_mgr_poll_s(ref(mg_mgr_s) mgr, int milli)
{
  return mg_mgr_poll(&get(mgr)->raw, milli);
}

void mg_serve_http_s(ref(mg_connection_s) nc, ref(http_message_s) hm,
  struct mg_serve_http_opts opts)
{
  mg_serve_http(get(nc)->raw, get(hm)->raw, opts);
}

ref(String) http_message_uri_s(ref(http_message_s) ctx)
{
  return get(ctx)->uri;
}

ref(String) http_message_query_string_s(ref(http_message_s) ctx)
{
  return get(ctx)->query_string;
}

//int mg_get_http_var_s(ref(CharArray), char *name, ref(CharArray) dest)
//{
//  TODO
//}

void mg_print_s(ref(mg_connection_s) nc, ref(CharArray) data)
{
  mg_send(get(nc)->raw, CharArrayGet(data), CharArraySize(data));
}
