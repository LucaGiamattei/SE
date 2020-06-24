CFLAGS=-g
DEPS=myuart.h utils.h
EXE_NAME_INT=uart_uio_with_int

all: main_int

main_int: main.c myuart_lib $(DEPS)
	$(CC) $(CFLAGS) -o $(EXE_NAME_INT) main.c myuart.o utils.o

myuart_lib: myuart.c utils.c $(DEPS)
	$(CC) -c myuart.c utils.c

clean: 
	rm *.o
	rm $(EXE_NAME_INT) 
