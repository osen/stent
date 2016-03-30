#include "gtk_stent.h"

void expired(gpointer data, GObject *where_the_object_was)
{
  _RefRelease(data);
}

