CFLAGS = -pthread

all: 1.1 1.2

1.1: 1.1.c
	${CC} ${CFLAGS} -o 1.1 $^
	
1.2: 1.2.c
	${CC} ${CFLAGS} -o 1.2 $^

clean:
	-rm -f *~ *.o 1.1 1.2