OBJS = \
	history.o\
	wpath.o\
	jobs.o\
	builtin.o\
	utils.o\
	shell.o\
	main.o

CFLAGS = -Wall -Og -g

CFLAGS += -D SHELL_DEBUG

shell: ${OBJS}
	cc ${OBJS} -o shell

main.o: main.c shell.h
	cc ${CFLAGS} -c main.c

shell.o: command.h builtincmd.h utils.h shell.h shell.c 
	cc ${CFLAGS} -c shell.c

builtin.o: jobs.h history.h wpath.h builtincmd.h builtincmd.c
	cc ${CFLAGS} -c builtincmd.c -o builtin.o

history.o: history.c history.h
	cc ${CFLAGS} -c history.c

wpath.o: utils.h wpath.h wpath.c
	cc ${CFLAGS} -c wpath.c

jobs.o : utils.h jobs.h jobs.c
	cc ${CFLAGS} -c jobs.c

utils.o: utils.c utils.h
	cc ${CFLAGS} -c utils.c

clean:
	rm *.o shell

