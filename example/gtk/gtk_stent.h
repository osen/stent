#ifndef GTK_STENT_H
#define GTK_STENT_H

#include <stent.h>
#include <gtk/gtk.h>

void expired(gpointer data, GObject *where_the_object_was);

#define G_OBJECT_ATTACH(R, P) \
  do \
  { \
    gpointer fTest = 0; \
    ATTACH_T(R, _GObject, P); \
    fTest = g_object_get_data(G_OBJECT(GET(R)), "_stent_func_set"); \
    if(fTest == 0) \
    { \
      g_object_weak_ref(G_OBJECT(GET(R)), expired, G_OBJECT(GET(R))); \
      g_object_set_data(G_OBJECT(GET(R)), "_stent_func_set", (gpointer)1); \
    } \
  } \
  while(0)

#define G_OBJECT_REATTACH(R, P) \
  REATTACH_T(R, _GObject, P)

REFDEF(_GtkApplication);
REFDEF(_GtkWindow);
REFDEF(_GtkWidget);

void quick_message(REF(_GtkWindow) parent, gchar *message);

#endif