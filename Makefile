CC=gcc 
CFLAGS=-Wall
main: pcm2pwm.o header.o convert.o
	gcc -o pcm2pwm pcm2pwm.o header.o convert.o

clean:
	rm -f  pcm2pwm.o header.o convert.o
	
clean-all:
	rm -f pcm2pwm pcm2pwm.o header.o convert.o