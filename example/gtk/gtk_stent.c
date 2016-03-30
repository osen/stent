#include "gtk_stent.h"

void expired(gpointer data, GObject *where_the_object_was)
{
  printf("Debug: G_OBJECT expired\n");
  _RefRelease(data);
}

void quick_message(REF(_GtkWindow) parent, gchar *message)
{
  REF(_GtkWidget) dialog = {};
  REF(_GtkWidget) label = {};
  REF(_GtkWidget) content_area = {};
  GtkDialogFlags flags = 0;

  // Create the widgets
  flags = GTK_DIALOG_DESTROY_WITH_PARENT;
  G_OBJECT_ATTACH(dialog, gtk_dialog_new_with_buttons("Message",
    GET(parent), flags, "_OK", GTK_RESPONSE_NONE, NULL));

  gtk_window_set_modal(GTK_WINDOW(GET(dialog)), 1);
  G_OBJECT_ATTACH(content_area, gtk_dialog_get_content_area(GTK_DIALOG(GET(dialog))));
  G_OBJECT_ATTACH(label, gtk_label_new(message));
  
  // Ensure that the dialog box is destroyed when the user responds
  g_signal_connect_swapped(GET(dialog),
    "response", G_CALLBACK(gtk_widget_destroy), GET(dialog));

  // Add the label, and show everything we've added
  gtk_container_add(GTK_CONTAINER(GET(content_area)), GET(label));
  gtk_widget_show_all(GET(dialog));
}
