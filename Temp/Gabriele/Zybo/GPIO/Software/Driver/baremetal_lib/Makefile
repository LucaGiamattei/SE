CC=gcc
CFLAGS=
DEPS = mygpio.h utils.h

test: test.c lib $(DEPS)
	$(CC) $(CFLAGS) -o test_lib test.c mygpio.o utils.o
	./test_lib


lib: mygpio.c utils.c
	$(CC) -c mygpio.c utils.c $(CFLAGS) 


clean: 
	rm *.o
	rm test_lib
