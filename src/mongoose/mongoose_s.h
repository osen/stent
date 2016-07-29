#ifndef MONGOOSE_S_H
#define MONGOOSE_S_H

#include "mongoose.h"

#include <stent.h>

refdef(mg_mgr_s);
refdef(mg_connection_s);
refdef(http_message_s);

ref(mg_mgr_s) mg_mgr_init_s(ref(Object) userData);

ref(mg_connection_s) mg_bind_s(ref(mg_mgr_s) mgr, char *interface,
  void (*func)(ref(mg_connection_s), int, ref(Object)));

void mg_set_protocol_http_websocket_s(ref(mg_connection_s) nc);

time_t mg_mgr_poll_s(ref(mg_mgr_s) mgr, int milli);

void mg_serve_http_s(ref(mg_connection_s) nc, ref(http_message_s) hm,
  struct mg_serve_http_opts opts);

ref(String) http_message_uri_s(ref(http_message_s) ctx);
ref(String) http_message_query_string_s(ref(http_message_s) ctx);

//int mg_get_http_var_s(ref(CharArray), char *name, ref(CharArray) dest);
void mg_print_s(ref(mg_connection_s), ref(CharArray));

#endif
