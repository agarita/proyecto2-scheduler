# proyecto2-scheduler
Proyecto 2 del curso de Sistemas Operativos.

## Requerimientos
Para poder compilar este proyecto se necesita la librería de graficos gtk. Pero se necesitan los archivos de desarrollo de gtk, sin ellos cmake no sabe como usar la librería.
Para instalar estos archivos con APT use:
`sudo apt-get install build-essential libgtk-3-dev`

## Comandos
Para construir el scheduler use:
`make`

Para limpiar el folder use:
`make clear`

Si desea compilar manualmente el programa use:
`` gcc -o run.o main.c gui.c \`pkg-config --cflags --libs gtk+-3.0\` ``
