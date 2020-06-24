CFLAGS=-g
DEPS=mygpio.h utils.h
EXE_NAME_NO_INT=uio_no_int
EXE_NAME_INT=uio_with_int

all: main_no_int main_int

main_int: main_int.c mygpio_lib $(DEPS)
	$(CC) $(CFLAGS) -o $(EXE_NAME_INT) main_int.c mygpio.o utils.o

main_no_int: main_no_int.c mygpio_lib $(DEPS)
	$(CC) $(CFLAGS) -o $(EXE_NAME_NO_INT) main_no_int.c mygpio.o utils.o

mygpio_lib: mygpio.c utils.c $(DEPS)
	$(CC) -c mygpio.c utils.c

clean: 
	rm *.o
	rm $(EXE_NAME_INT) $(EXE_NAME_NO_INT) 
