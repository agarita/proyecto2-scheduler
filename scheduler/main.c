#include "main.h"

static void activate (GtkApplication* app, gpointer user_data){
  GtkWidget *window;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
  gtk_widget_show_all (window);
}

double taylor(int n){
  double sum = 1.0;
  int sign = -1, i = 1;

  for(i = 1; i < n; i++){
    sum += sign / (2.0 * i + 1.0);
    printf("%f\n", 4.0*sum);
    sign = -sign;
  }
  return 4.0*sum;
}

int main(int argc, char *argv[]) {
  taylor(2000);

  /*GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run(G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;*/
}
