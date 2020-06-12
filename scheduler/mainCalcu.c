#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

static GtkWidget *number1;
static GtkWidget *number2;
static GtkWidget *result;

static GtkWidget *algoritmo;
static GtkWidget *modoOperacion;
static GtkWidget *quantum;
static GtkWidget *avance;
static GtkWidget *proceso;

void do_change(GtkWidget *calculate) {
    gtk_label_set_text(GTK_LABEL(result), "Resultado cambia texto");
}

// gcc 007_gtk.c -o 007_gtk `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`
int main(int argc, char **argv) {
    GtkWidget *window, *grid, *ejecutar, *insertar, *reiniciar, *limpiar;
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    algoritmo = gtk_label_new("Algoritmo:");
    gtk_grid_attach(GTK_GRID(grid), algoritmo, 0, 0, 1, 1);

    modoOperacion = gtk_label_new("Modo de Operacion:");
    gtk_grid_attach(GTK_GRID(grid), modoOperacion, 0, 1, 1, 1);

    quantum = gtk_label_new("Quantum/Trabajo:");
    gtk_grid_attach(GTK_GRID(grid), quantum, 0, 2, 1, 1);

    avance = gtk_label_new("Avance:");
    gtk_grid_attach(GTK_GRID(grid), avance, 0, 3, 1, 1);

    ejecutar = gtk_button_new_with_label("ejecutar");
    g_signal_connect(ejecutar, "clicked", G_CALLBACK(do_change), NULL);
    gtk_grid_attach(GTK_GRID(grid), ejecutar, 2,15, 1, 1);

    insertar = gtk_button_new_with_label("insertar");
    g_signal_connect(insertar, "clicked", G_CALLBACK(do_change), NULL);
    gtk_grid_attach(GTK_GRID(grid), insertar, 3, 15, 1, 1);

    reiniciar = gtk_button_new_with_label("reiniciar");
    g_signal_connect(reiniciar, "clicked", G_CALLBACK(do_change), NULL);
    gtk_grid_attach(GTK_GRID(grid), reiniciar, 2, 16, 1, 1);

    limpiar = gtk_button_new_with_label("limpiar");
    g_signal_connect(limpiar, "clicked", G_CALLBACK(do_change), NULL);
    gtk_grid_attach(GTK_GRID(grid), limpiar, 3, 16, 1, 1);

    proceso = gtk_label_new("proceso:");
    gtk_grid_attach(GTK_GRID(grid), proceso, 3,3, 1, 1);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}