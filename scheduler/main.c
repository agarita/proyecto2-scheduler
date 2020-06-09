#include "main.h"

/*--------
 INTERFAZ
--------*/
static void activate (GtkApplication* app, gpointer user_data){
  GtkWidget *window;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
  gtk_widget_show_all (window);
}

/*-------------
 TAYLOR SERIES
-------------*/
void arcsin(int n){
  mpfr_t res; // arcsin(1)
  mpfr_t pi;  // 2*res

  mpfr_t u;   // (2n)!

  mpfr_t a;   // 4^n
  mpfr_t b;   // (n!)^2
  mpfr_t c;   // 2n+1
  mpfr_t d;   // a*b*c

  mpfr_t t;   // t = u/d

  mpfr_inits2 (256, res, pi, u, a, b, c, d, t, (mpfr_ptr) 0);

  mpfr_set_d (res, 1.0, MPFR_RNDD); mpfr_set_d (pi, 1.0, MPFR_RNDD);

  mpfr_set_d (a, 1.0, MPFR_RNDD); mpfr_set_d (b, 1.0, MPFR_RNDD);
  mpfr_set_d (c, 1.0, MPFR_RNDD); mpfr_set_d (d, 1.0, MPFR_RNDD);
  mpfr_set_d (u, 1.0, MPFR_RNDD); mpfr_set_d (t, 1.0, MPFR_RNDD);

  for (n = 1; n <= 50*n; n++){
    for(int i = 1; i <= 2*n; i++){
      mpfr_mul_ui (u, u, i, MPFR_RNDU); // (2n)!
    }
    //----------------------------------
    mpfr_mul_ui (a, a, 4, MPFR_RNDD);
    mpfr_pow_ui (a, a, n, MPFR_RNDD);   // 4^n
    //**********************************
    for(int i = 1; i <= n; i++){
      mpfr_mul_ui (b, b, i, MPFR_RNDU); // n!
    }
    mpfr_pow_ui (b, b, 2, MPFR_RNDD);   // b^2
    //**********************************
    mpfr_mul_ui (c, c, 2*n, MPFR_RNDD);
    mpfr_add_ui (c, c, 1, MPFR_RNDD);   // 2n+1
    //==================================
    mpfr_mul (d, a, b, MPFR_RNDD);
    mpfr_mul (d, d, c, MPFR_RNDD);
    ////////////////////////////////////
    mpfr_div (t, u, d, MPFR_RNDD);

    mpfr_add (res, res, t, MPFR_RNDD);
    mpfr_mul_ui (pi, res, 2, MPFR_RNDD);

    printf("PI = ~");
    mpfr_out_str (stdout, 10, 0, pi, MPFR_RNDD);
    putchar('\n');

    mpfr_set_d (a, 1.0, MPFR_RNDD); mpfr_set_d (b, 1.0, MPFR_RNDD);
    mpfr_set_d (c, 1.0, MPFR_RNDD); mpfr_set_d (d, 1.0, MPFR_RNDD);
    mpfr_set_d (u, 1.0, MPFR_RNDD); mpfr_set_d (t, 1.0, MPFR_RNDD);
  }

  mpfr_clears (res, a, b, c, u, d, t, (mpfr_ptr) 0);
  mpfr_free_cache ();
}

/*-------------
     MAIN
-------------*/
int main(int argc, char *argv[]) {
  arcsin(3);
  return 0;

  /*GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run(G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;*/
}
