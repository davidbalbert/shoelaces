CC=gcc-4.2
CFLAGS=-g

shoelaces: shoelaces.c

run-tests: test
	./test

test: test.o init.o type.o number.o reader.o

test.o: test.c shoelaces.h

init.o: init.c shoelaces.h

type.o: type.c shoelaces.h

reader.o: reader.c shoelaces.h

number.o: number.c shoelaces.h

clean:
	rm -rf *.o *.dSYM shoelaces test

.PHONY: clean
