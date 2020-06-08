# proyecto2-scheduler
Proyecto 2 del curso de Sistemas Operativos.

## Requerimientos
Se ocupan tener instaladas ciertas librerías para poder ejecutar este proyecto satisfactoriamente: Estas librerías son:
 *  [GNU Multiple Precision Arithmetic Library](https://gmplib.org/)
 *  [GNOME ToolKit Graphical Library](https://www.gtk.org/)

### Instalación GMP
Se debe descargar GMP de la web oficial y descomprimirlo en el lugar de preferencia. Hecho esto se puede proceder a la configuración desde la terminal con:

    ./configure
    make

Si se desea se pueden realizar pruebas, para confirmar que la instalación es correcta, por medio de:

    make check

### Instalación GTK
Se necesitan instalar los archivos de desarrollo de gtk, sin ellos cmake no sabe como usar la librería. Para instalar estos archivos con APT use:

    sudo apt-get install build-essential libgtk-3-dev
---
## Comandos
Para construir el scheduler use:

    make

Para limpiar el folder use:

    make clear

Si desea compilar manualmente el programa use:

    gcc -o run.o main.c gui.c `pkg-config --cflags --libs gtk+-3.0`
