#ifndef GTK_STENT_H
#define GTK_STENT_H

#include <stent.h>
#include <gtk/gtk.h>

void expired(gpointer data, GObject *where_the_object_was);

#define G_OBJECT_ATTACH(R, P) \
  do \
  { \
    ATTACH(R, P); \
    g_object_weak_ref(G_OBJECT(GET(R)), expired, G_OBJECT(GET(R))); \
  } \
  while(0)

DECLARE(_GtkApplication);
DECLARE(_GtkWidget);

#endif
