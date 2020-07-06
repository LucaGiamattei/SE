CFLAGS=-DMYGPIO_NO_DRIVER
DEPS=mygpio.h utils.h
EXE_NAME=led_noDriver

all: main.c mygpio_lib $(DEPS)
	$(CC) $(CFLAGS) -o $(EXE_NAME) main.c mygpio.o utils.o

mygpio_lib: mygpio.c utils.c $(DEPS)
	$(CC) $(CFLAGS) -c mygpio.c utils.c

clean: 
	rm *.o
	rm $(EXE_NAME)
