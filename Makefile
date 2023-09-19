CC=gcc
CFLAGS=-g -Wall -pedantic -Wimplicit-function-declaration
OLIBSFLAGS=-lreadline -lhistory
LIBFLAGS=$(OLIBSFLAGS)

all: 	gosh	/


gosh:   gosh.o
	$(CC) $(CFLAGS) -o $@ gosh.o $(LIBFLAGS)

# generic
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f pipe
	rm -f gosh
	rm -f *.o
	rm -f rlbasic
	rm -f histexamp
	rm -f 00_execvp_example
	rm -f 00_fork_dup_pipe_exec
