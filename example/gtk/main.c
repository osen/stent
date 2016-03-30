#include "gtk_stent.h"

struct WindowData
{
  REF(_GtkWindow) window;
  int a;
};

DECLARE(WindowData);

static void print_hello(struct _GtkWidget *widget, gpointer data)
{
  REF(_GtkWidget) owner = {};
  REF(WindowData) windowData = {};
  REF(_GtkWindow) window = {};

  G_OBJECT_ATTACH(owner, widget);

  // REATTACH used to ensure type safe conversion from "gpointer" to "WindowData".
  REATTACH(windowData, WindowData, data);

  G_OBJECT_ATTACH(window, GTK_WINDOW(gtk_widget_get_toplevel(GET(owner))));

  if(GET(windowData) == NULL)
  {
    printf("No data was passed, invalid type or freed\n");
    quick_message(window, "No data was passed, invalid type or freed");
    return;
  }

  printf("Number should be 8: %i\n", GET(windowData)->a);
  printf("I am now going to free this data so try clicking the button again\n");

  quick_message(GET(windowData)->window, "I am now going to free this data so try clicking the button again");

  FREE(windowData);
}

static void activate(GtkApplication *app, gpointer user_data)
{
  REF(_GtkWindow) window = {};
  REF(_GtkWidget) button = {};
  REF(_GtkWidget) button_box = {};
  REF(WindowData) data = {};

  G_OBJECT_ATTACH(window, GTK_WINDOW(gtk_application_window_new(app)));
  gtk_window_set_title(GTK_WINDOW(GET(window)), "Window");
  gtk_window_set_default_size(GTK_WINDOW(GET(window)), 200, 200);

  G_OBJECT_ATTACH(button_box, gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_container_add(GTK_CONTAINER(GET(window)), GET(button_box));

  data = CALLOC(WindowData);
  GET(data)->a = 8;
  GET(data)->window = window;

  G_OBJECT_ATTACH(button, gtk_button_new_with_label("Hello World"));
  g_signal_connect(GET(button), "clicked", G_CALLBACK(print_hello), GET(data));
  //g_signal_connect_swapped(GET(button), "clicked", G_CALLBACK(gtk_widget_destroy), GET(window));
  gtk_container_add(GTK_CONTAINER(GET(button_box)), GET(button));

  gtk_widget_show_all(GTK_WIDGET(GET(window)));
}

int main(int argc, char **argv)
{
  int status = 0;
  REF(_GtkApplication) app = {};

  G_OBJECT_ATTACH(app, gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE));

  g_signal_connect(GET(app), "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(GET(app)), argc, argv);

  printf("Before: %p\n", GET(app));

  // Free all GTK+ allocated resources.
  g_object_unref(GET(app));

  // Any reference containing the freed "app" instance will now be NULL.
  printf("After: %p\n", GET(app));

  // Inform us of any memory leaks in the program.
  // We have 1 leak in this program unless button was clicked.
  RefStats();
  RefCleanup();

  return status;
}
