CC=gcc
CFLAGS=
DEPS = myuart.h utils.h

test: test.c lib $(DEPS)
	$(CC) $(CFLAGS) -o test_lib test.c myuart.o utils.o
	./test_lib


lib: myuart.c utils.c
	$(CC) -c mygpio.c utils.c $(CFLAGS) 


clean: 
	rm *.o
	rm test_lib
