CC=arm-buildroot-linux-uclibcgnueabihf-gcc
SHELL=/bin/bash



CFLAGS=
DEPS = mygpio.h utils.h


main: main.c lib $(DEPS)
	
	$(CC) $(CFLAGS) -o led_nodriver main.c mygpio.o utils.o


lib: mygpio.c utils.c
	
	$(CC) -c mygpio.c utils.c $(CFLAGS) 


clean: 
	rm *.o
	rm led_nodriver
