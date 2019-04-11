CC = mpicc
CFLAGS = -std=c99 -fopenmp -g 

donde: donde.c
	${CC} -o donde ${CFLAGS} $<

clean:
	rm -f *.o donde
