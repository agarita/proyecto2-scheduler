cli_exec:
	gcc -o run.o main.c -lmpfr -lgmp `pkg-config --cflags --libs gtk+-3.0`
	./run.o

clean:
	rm -rf *.o *.bin *.elf
