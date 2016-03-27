#include <gtk/gtk.h>
#include <stent.h>

void expired(gpointer data, GObject *where_the_object_was)
{
  _RefRelease(data);
}

struct _GObject *_CreateGObjectRef(struct _GObject *obj)
{
  g_object_weak_ref(obj, expired, obj);

  return obj;
}

#define G_OBJECT_REF(T, P) \
  ATTACH(T, _CreateGObjectRef(G_OBJECT(P)))

DECLARE(_GtkApplication);
DECLARE(_GtkWidget);

static void print_hello(struct _GtkWidget *widget, gpointer data)
{
  g_print("Hello World\n");
}

static void activate(GtkApplication *app, gpointer user_data)
{
  REF(_GtkWidget) window = {};
  REF(_GtkWidget) button = {};
  REF(_GtkWidget) button_box = {};

  window = G_OBJECT_REF(_GtkWidget, gtk_application_window_new(app));
  gtk_window_set_title(GTK_WINDOW(GET(window)), "Window");
  gtk_window_set_default_size(GTK_WINDOW(GET(window)), 200, 200);

  button_box = G_OBJECT_REF(_GtkWidget, gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_container_add(GTK_CONTAINER(GET(window)), GET(button_box));

  button = G_OBJECT_REF(_GtkWidget, gtk_button_new_with_label("Hello World"));
  g_signal_connect(GET(button), "clicked", G_CALLBACK(print_hello), NULL);
  g_signal_connect_swapped(GET(button), "clicked", G_CALLBACK(gtk_widget_destroy), GET(window));
  gtk_container_add(GTK_CONTAINER(GET(button_box)), GET(button));

  gtk_widget_show_all(GET(window));
}

int main(int argc, char **argv)
{
  int status = 0;
  REF(_GtkApplication) app = {};

  app = G_OBJECT_REF(_GtkApplication, gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE));

  g_signal_connect(GET(app), "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(GET(app)), argc, argv);

  printf("Before: %p\n", GET(app));
  g_object_unref(GET(app));
  printf("After: %p\n", GET(app));

  RefStats();
  RefCleanup();

  return status;
}
