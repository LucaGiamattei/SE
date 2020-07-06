CFLAGS=-DMYUART_NO_DRIVER
DEPS=myuart.h utils.h
EXE_NAME=uart_noDriver

all: main.c myuart_lib $(DEPS)
	$(CC) $(CFLAGS) -o $(EXE_NAME) main.c myuart.o utils.o

myuart_lib: myuart.c utils.c $(DEPS)
	$(CC) $(CFLAGS) -c myuart.c utils.c

clean: 
	rm *.o
	rm $(EXE_NAME)
