OBJS = \
	history.o\
	wpath.o\
	jobs.o\
	builtin.o\
	utils.o\
	shell.o\
	main.o

CFLAGS = -Wall -O2

CFLAGS += -D SHELL_DEBUG

shell: ${OBJS}
	${CC} ${OBJS} -o shell

main.o: main.c shell.h
	${CC} ${CFLAGS} -c main.c

shell.o: command.h builtincmd.h utils.h shell.h shell.c 
	${CC} ${CFLAGS} -c shell.c

builtin.o: jobs.h history.h wpath.h builtincmd.h builtincmd.c
	${CC} ${CFLAGS} -c builtincmd.c -o builtin.o

history.o: history.c history.h
	${CC} ${CFLAGS} -c history.c

wpath.o: utils.h wpath.h wpath.c
	${CC} ${CFLAGS} -c wpath.c

jobs.o : utils.h jobs.h jobs.c
	${CC} ${CFLAGS} -c jobs.c

utils.o: utils.c utils.h
	${CC} ${CFLAGS} -c utils.c

clean:
	rm *.o shell
