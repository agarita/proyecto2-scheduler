# proyecto2-scheduler
Proyecto 2 del curso de Sistemas Operativos.

## Requerimientos
Se ocupan tener instaladas ciertas librerías para poder ejecutar este proyecto satisfactoriamente: Estas librerías son:
 *  [GNOME ToolKit Graphical Library](https://www.gtk.org/)
 *  [GNU Multiple Precision Arithmetic Library](https://gmplib.org/)
 *  [GNU Multiple Precision Floating-Point Reliable Library](https://www.mpfr.org/)

### Instalación GTK
Se necesitan instalar los archivos de desarrollo de gtk, sin ellos cmake no sabe como usar la librería. Para instalar estos archivos con APT use:

    sudo apt-get install build-essential libgtk-3-dev

### Instalación GMP
Se debe descargar GMP de la web oficial y descomprimirlo en el lugar de preferencia. Hecho esto se puede proceder a la configuración desde la terminal con:

    ./configure
    make

Si se desea se pueden realizar pruebas, para confirmar que la instalación es correcta, por medio de:

    make check

### Instalación MPFR
Para instalar MPFR se debe tener previamente instalado GMP (versión 5.0.0 o superior). Tan solo se debe descargar MPFR de la web oficial y descomprimirlo en el lugar de preferencia. Hecho esto se procede a la configuración desde la terminal con:

    ./configure
    make

Si se desea se pueden realizar pruebas, para confirmar que la instalación es correcta, por medio de:

    make check
---
## Comandos
Para construir el scheduler use:

    make

Para limpiar el folder use:

    make clear

Si desea compilar manualmente el programa use:

    gcc -o run.o main.c -lmpfr -lgmp `pkg-config --cflags --libs gtk+-3.0`
    
Solo la interfaz

	gcc -o run.o mainCalc.c `pkg-config --cflags --libs gtk+-3.0`
    ./run.o
