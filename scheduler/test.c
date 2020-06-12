PCB{
  PID
  Cantidad de terminos totales
  Cantidad de terminos actuales
  Aproximación de pi
}

TIMER

TS(Aproximación de pi, cantidad de terminos ){

}

while(){
  if(wait(TRABAJO_A_REALIZAR/Quantum) || trabajoCompletado){
      /*
      * Ejecuta Algoritmo de Calendarización
      */
  }
}

#include <gtk/gtk.h>

int main(int argc, char *argv[]) {

  GtkWidget *window;
  GtkWidget *buttonEjecutar;
  GtkWidget *halign;

  gtk_init(&argc, &argv);

//Window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Scheduler");
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
  gtk_container_set_border_width(GTK_CONTAINER(window), 15);


  //ComboBox

  //CreateWindow("COMBOBOX","",CBS_DROPDOWNLIST|WS_CHILD|WS_VISIBLE,340,120,100,500,hwnd,(HMENU)ID_COMBOBOX,NULL,NULL);
  //  for(int i=0;i<nitems;i++){
  //    SendDlgItemMessage(hwnd,ID_COMBOBOX,CB_ADDSTRING,0,(LPARAM)operaciones[i]);
  //    SendDlgItemMessage(hwnd,ID_COMBOBOX,CB_SETCURSEL,(WPARAM)3,(LPARAM)0);
  //}

  //Buttons

  //Ejecutar
  buttonEjecutar = gtk_button_new_with_label("Ejecutar");
  gtk_widget_set_tooltip_text(buttonEjecutar, "Ejecutar algoritmos");

  //Reiniciar
  //button = gtk_button_new_with_label("Reiniciar");
  //gtk_widget_set_tooltip_text(button, "Reiniciar esta ejecucion");
  //Insert
  //button = gtk_button_new_with_label("Insertar");
  //gtk_widget_set_tooltip_text(button, "Insertar lista de algoritmos");
  //Limpiar todo
  //button = gtk_button_new_with_label("Limpiar Todo");
  //gtk_widget_set_tooltip_text(button, "Limpiar toda la memoria");

  //Haling
  halign = gtk_alignment_new(0, 0, 0, 0);
  gtk_container_add(GTK_CONTAINER(halign), buttonEjecutar);
  gtk_container_add(GTK_CONTAINER(window), halign);

  gtk_widget_show_all(window);

  g_signal_connect(G_OBJECT(window), "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_main();

  return 0;
}
