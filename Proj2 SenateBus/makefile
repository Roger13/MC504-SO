CC = gcc
CFLAGS = -g -pthread 

PROGRAMS = main render

main: main.c render.h render.c
	gcc -g -pthread -o main main.c render.c
clean:
	rm -f *- $(PROGRAMS)
