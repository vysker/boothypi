#include <gtk/gtk.h>

const gchar preview_image_path[] = "/home/dick/dev/pi/boothypiui-gtk/preview.jpg";
int preview_image_width = 600;
int preview_image_height = 400;
GtkWidget *image;

static void hello(GtkWidget *widget, gpointer data)
{
  g_print("Hello\n");
}

static void destroy(GtkWidget *widget, gpointer data)
{
  gtk_main_quit();
}

static gboolean load_preview_image(gpointer image)
{
  GdkPixbuf *pixbuf;
  GdkPixbuf *pixbuf2;
  GError *error = NULL;

  pixbuf = gdk_pixbuf_new_from_file(preview_image_path, &error);
  if (error == NULL) {
    pixbuf2 = gdk_pixbuf_scale_simple(pixbuf, preview_image_width, preview_image_height, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf2);
    g_clear_object(&pixbuf2);
  } else {
    // g_printf("Pixbuf load error: %s\n", error->message);
  }

  g_clear_error(&error);
  g_clear_object(&pixbuf);

  return TRUE;
}

static gpointer stream_preview_image(gpointer image)
{
  int num_errors = 0;
  int num_images_loaded = 0;
  int i = 0;

  // g_timeout_add is like setInterval in Javascript.
  int timeout_id = g_timeout_add(250, load_preview_image, image); // timeout_id can be used to destroy the interval.

  return NULL;
}

int main(int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *box1;

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

  image = gtk_image_new();
  gtk_box_pack_start(GTK_BOX(box1), image, TRUE, TRUE, 0);
  gtk_widget_show(image);

  gtk_widget_show(box1);
  gtk_widget_show(window);

  g_thread_new("StreamPreviewImage", stream_preview_image, image);

  gtk_main();

  return 0;
}
