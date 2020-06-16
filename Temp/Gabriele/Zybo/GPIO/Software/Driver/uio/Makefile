CC=arm-buildroot-linux-uclibcgnueabihf-gcc
SHELL=/bin/bash



CFLAGS=
DEPS = mygpio.h utils.h


main_no_int: main_no_int.c lib $(DEPS)
	
	$(CC) $(CFLAGS) -o led_uio_no_int main_no_int.c mygpio.o utils.o


lib: mygpio.c utils.c
	
	$(CC) -c mygpio.c utils.c $(CFLAGS) 


clean: 
	rm *.o
	rm led_uio_no_int
