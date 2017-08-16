#include <gtk/gtk.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>

const gchar preview_image_path[] = "/home/dick/dev/pi/boothypiui-gtk/preview.jpg";
const char roll_images_dir[] = "/home/dick/dev/pi/boothypiui-gtk/roll";
int preview_image_width = 600;
int preview_image_height = 400;
int preview_stream_interval = 250;
int roll_image_width = 600;
int roll_image_height = 400;
GtkWidget *preview_image;

#define roll_image_count 3
GtkImage * *roll_image;
GtkImage * roll_images[roll_image_count];

static void destroy(GtkWidget *widget, gpointer data)
{
  gtk_main_quit();
}

static gboolean load_preview_image(gpointer data)
{
  GdkPixbuf *pixbuf;
  GdkPixbuf *pixbuf2;
  GError *error = NULL;

  pixbuf = gdk_pixbuf_new_from_file(preview_image_path, &error);
  if (error == NULL) {
    pixbuf2 = gdk_pixbuf_scale_simple(pixbuf, preview_image_width, preview_image_height, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(preview_image), pixbuf2);
    g_clear_object(&pixbuf2);
  } else {
    // g_printf("Pixbuf load error: %s\n", error->message);
  }

  g_clear_error(&error);
  g_clear_object(&pixbuf);

  return TRUE;
}

static void shift_roll_images()
{
  printf("shifting start\n");
  for(int i = roll_image_count - 1; i > 0; i--) {
    printf("i: %i\n", i);

    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(roll_images[i - 1]);
    if (pixbuf != NULL) {
      printf("setting %i to %i\n", i-1, i);
      gtk_image_set_from_pixbuf(roll_images[i], pixbuf);
    }
  }

  // roll_images[0] = (GtkImage *) gtk_image_new();
  printf("shifting done\n");
}

static gboolean load_roll_image(gpointer data)
{
  GdkPixbuf *pixbuf;
  GdkPixbuf *pixbuf2;
  GError *error = NULL;

  char filename[strlen(data)];
  strcpy(filename, data);

  printf("filename %s\n", filename);

  char roll_image_path[strlen(roll_images_dir) + strlen(filename)];
  strcpy(roll_image_path, roll_images_dir);
  strcat(roll_image_path, "/");
  strcat(roll_image_path, filename);

  printf("full path %s\n", roll_image_path);

  pixbuf = gdk_pixbuf_new_from_file(roll_image_path, &error);
  printf("newpixbuf\n");
  if (error == NULL) {
    shift_roll_images();

    pixbuf2 = gdk_pixbuf_scale_simple(pixbuf, roll_image_width, roll_image_height, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(roll_images[0]), pixbuf2);
    g_clear_object(&pixbuf2);
  } else {
    // g_printf("Pixbuf load error: %s\n", error->message);
  }

  printf("roll image loaded\n");

  g_clear_error(&error);
  g_clear_object(&pixbuf);

  return TRUE;
}

static gpointer stream_preview_image(gpointer data)
{
  // g_timeout_add is like setInterval in Javascript.
  g_timeout_add(preview_stream_interval, load_preview_image, NULL);

  return NULL;
}

static gpointer stream_roll_images(gpointer data)
{
  int fd = inotify_init();
  int wd = inotify_add_watch(fd, roll_images_dir, IN_CREATE); // In case of multiple masks, pass them like so: IN_CREATE | IN_MODIFY | IN_DELETE

  size_t event_size = sizeof(struct inotify_event) + NAME_MAX + 1;
  size_t buffer_len = event_size * 1024;

  // struct inotify_event {
  //   int      wd;       /* Watch descriptor */
  //   uint32_t mask;     /* Mask of events */
  //   uint32_t cookie;   /* Unique cookie associating related
  //                         events (for rename(2)) */
  //   uint32_t len;      /* Size of name field */
  //   char     name[];   /* Optional null-terminated name */
  // };

  while (TRUE) {
    char buffer[buffer_len];

    int bytes_read = read(fd, buffer, buffer_len);

    if (bytes_read > 0) {
      struct inotify_event *event_buf = (struct inotify_event *) &buffer;

      if (event_buf->len && (event_buf->mask & IN_CREATE) && (event_buf->mask & IN_ISDIR) == 0) {
        load_roll_image(event_buf->name);
      }
    }
  }

  inotify_rm_watch(fd, wd);
  close(fd);

  return NULL;
}

int main(int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *root_box;
  GtkWidget *stream_box;
  GtkWidget *roll_box;
  GtkWidget *info_box;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "BoothyPi");
  g_signal_connect(window, "delete_event", G_CALLBACK(destroy), NULL);

  gtk_container_set_border_width(GTK_CONTAINER(window), 10);

  root_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  stream_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  roll_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  gtk_container_add(GTK_CONTAINER(window), root_box);

  preview_image = gtk_image_new();
  gtk_box_pack_start(GTK_BOX(stream_box), preview_image, TRUE, TRUE, 0);

  for(int i = 0; i < roll_image_count; i++) {
    roll_images[i] = (GtkImage *) gtk_image_new();
    gtk_box_pack_start(GTK_BOX(roll_box), GTK_WIDGET(roll_images[i]), TRUE, TRUE, 0);
  }

  gtk_box_pack_start(GTK_BOX(stream_box), roll_box, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(root_box), stream_box, TRUE, TRUE, 0);

  gtk_widget_show_all(window);

  g_thread_new("StreamPreviewImage", stream_preview_image, NULL);
  g_thread_new("StreamRollImages", stream_roll_images, NULL);

  gtk_main();

  return 0;
}
