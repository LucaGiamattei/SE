CFLAGS=-DMYGPIO_KERNEL
DEPS=mygpio.h config.h 
EXE_NAME=kernel_test

all: main.c lib  $(DEPS)
	$(CC) $(CFLAGS) -o $(EXE_NAME) main.c mygpio.o utils.o 

lib: $(DEPS) mygpio.c utils.c
	$(CC) $(CFLAGS) -c mygpio.c utils.c
clean: 
	rm *.o
	rm $(EXE_NAME)