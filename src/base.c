#include <gtk/gtk.h>

// Requires: sudo apt-get install libgtk-3-dev
// Then reboot

static void hello(GtkWidget *widget, gpointer data)
{
  g_print("Hello\n");
}

static void destroy(GtkWidget *widget, gpointer data)
{
  gtk_main_quit();
}

int main(int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *box1;
  GtkWidget *image;
  GdkPixbuf *pixbuf;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "BoothyPi");
  g_signal_connect(window, "delete_event", G_CALLBACK(destroy), NULL);

  gtk_container_set_border_width(GTK_CONTAINER(window), 10);

  box1 = gtk_box_new(FALSE, 0);

  gtk_container_add(GTK_CONTAINER(window), box1);

  button = gtk_button_new_with_label("Hello");
  g_signal_connect(button, "clicked", G_CALLBACK(hello), NULL);
  gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
  gtk_widget_show(button);

  // pixbuf = gdk_pixbuf_new_from_file("/home/dick/dev/pi/boothypiui-gtk/preview.jpg", NULL);
  // pixbuf = gdk_pixbuf_scale_simple(pixbuf, 600, 400, GDK_INTERP_BILINEAR);
  pixbuf = gdk_pixbuf_new_from_file_at_scale("/home/dick/dev/pi/boothypiui-gtk/preview.jpg", 100, 300, TRUE, NULL);
  image = gtk_image_new_from_pixbuf(pixbuf);
  // image = gtk_image_new_from_file("/home/dick/dev/pi/boothypiui-gtk/preview.jpg");
  gtk_box_pack_start(GTK_BOX(box1), image, TRUE, TRUE, 0);
  gtk_widget_show(image);

  gtk_widget_show(box1);
  gtk_widget_show(window);

  gtk_main();

  return 0;
}
