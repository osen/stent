/******************************************************************************
 *
 ******************************************************************************/

#include "gtk_stent.h"

struct WindowData
{
  REF(_GtkWindow) window; // A "smart" reference to window in context
  int a;                  // Some random data to test
};

REFDEF(WindowData); // Provides the "smart" definition of REF(WindowData)

static void print_hello(struct _GtkWidget *widget, gpointer data)
{
  REF(_GtkWidget) owner = {};
  REF(WindowData) windowData = {};
  REF(_GtkWindow) window = {};

  // Assign this to a "smart" reference early to avoid it becoming invalid
  G_OBJECT_REATTACH(owner, widget);

  // REATTACH ensures typesafe conversion from "gpointer" to "WindowData"
  REATTACH_T(windowData, WindowData, data);

  // Obtain toplevel of widget to open message dialog
  G_OBJECT_REATTACH(window, GTK_WINDOW(gtk_widget_get_toplevel(GET(owner))));

  // Check to see if "WindowData" is dangling and invalid
  if(GET(windowData) == NULL)
  {
    printf("No data was passed, invalid type or freed\n");
    quick_message(window, "No data was passed, invalid type or freed");
    return;
  }

  printf("Number should be 8: %i\n", GET(windowData)->a);
  printf("I am now going to free this data so try clicking the button again\n");

  // Use the parent window reference from the user data. With stent, it is
  // impossible to have a dangling pointer so will always be valid or NULL
  // causing a clean debuggable crash instead
  quick_message(GET(windowData)->window,
    "I am now going to free this data so try clicking the button again");

  // Delete "WindowData" to attempt to cause a dangling pointer
  FREE(windowData);
}

static void activate(GtkApplication *app, gpointer user_data)
{
  REF(_GtkApplication) _app = {};
  REF(_GtkWindow) window = {};
  REF(_GtkWidget) button = {};
  REF(_GtkWidget) button_box = {};
  REF(WindowData) data = {};

  // Make sure that app ptr is valid by reattaching to the previous "smart"
  // reference created in main
  G_OBJECT_REATTACH(_app, app);

  // Create GtkWindow and set some attributes
  G_OBJECT_ATTACH(window, GTK_WINDOW(gtk_application_window_new(GET(_app))));
  gtk_window_set_title(GTK_WINDOW(GET(window)), "Window");
  gtk_window_set_default_size(GTK_WINDOW(GET(window)), 200, 200);

  // Create layout container for our button and add to window
  G_OBJECT_ATTACH(button_box, gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_container_add(GTK_CONTAINER(GET(window)), GET(button_box));

  // Allocate our "WindowData" and populate it with some useful things
  data = CALLOC(WindowData);
  GET(data)->a = 8;
  GET(data)->window = window;

  // Create GtkButton and set the clicked callback, using our "WindowData"
  // structure as user data
  G_OBJECT_ATTACH(button, gtk_button_new_with_label("Hello World"));
  g_signal_connect(GET(button), "clicked", G_CALLBACK(print_hello), GET(data));

  // Try passing an invalid user data pointer
  //g_signal_connect(GET(button), "clicked", G_CALLBACK(print_hello), (void*)0xDEADBEEF);

  // Add button to layout container and show window
  gtk_container_add(GTK_CONTAINER(GET(button_box)), GET(button));
  gtk_widget_show_all(GTK_WIDGET(GET(window)));
}

int main(int argc, char **argv)
{
  int status = 0;
  REF(_GtkApplication) app = {};

  // Create an add new GtkApplication to "smart" reference. This reference
  // will become NULL if the GtkApplication ever gets freed
  G_OBJECT_ATTACH(app, gtk_application_new("stent.gtk_example",
    G_APPLICATION_FLAGS_NONE));

  // Set activate callback and enter GUI main loop
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
