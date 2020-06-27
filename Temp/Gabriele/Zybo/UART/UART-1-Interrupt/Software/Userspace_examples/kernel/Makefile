CFLAGS=-DMYUART_KERNEL
DEPS=myuart.h config.h 
EXE_NAME=kernel_test

all: main.c lib  $(DEPS)
	$(CC) $(CFLAGS) -o $(EXE_NAME) main.c myuart.o utils.o 

lib: $(DEPS) myuart.c utils.c
	$(CC) $(CFLAGS) -c myuart.c utils.c
clean: 
	rm *.o
	rm $(EXE_NAME)
