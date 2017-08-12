#include <gtk/gtk.h>

// Requires: sudo apt-get install libgtk-3-dev
// Then reboot

int main(int argc, char *argv[])
{
  GtkWidget *window;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_show(window);

  gtk_main();

  return 0;
}