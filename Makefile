CC=gcc 
CFLAGS=-Wall
main: pcm2pwm.o convert.o
	gcc -o pcm2pwm pcm2pwm.o convert.o
	
clean:
	rm -f pcm2pwm pcm2pwm.o convert.o