CC=gcc
CFLAGS=-Wall -Werror -g
	
default: SEEsh
	
SEEsh.o: SEEsh.c
	$(CC) $(CFLAGS) -c SEEsh.c -o SEEsh.o

SEEsh: SEEsh.o
	$(CC) $(CFLAGS) SEEsh.o -o SEEsh
	
run: SEEsh
	./SEEsh

clean:
	rm SEEsh.o
	rm SEEsh

