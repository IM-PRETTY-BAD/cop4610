CFLAGS = -pthread

all: part1.c
	${CC} ${CFLAGS} -o part1 $^
	${CC} ${CFLAGS} -DPTHREAD_SYNC -o part1-sync $^
	${CC} ${CFLAGS} -DPTHREAD_SYNC -DPTHREAD_INSPECT -o part1-inspect $^

clean:
	-rm -f *~ *.o part1 part1-sync part1-inspect
