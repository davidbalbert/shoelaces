CC=clang
CFLAGS=-g

shoelaces: shoelaces.c

run-tests: test
	./test

.PHONY: run-tests

test: test.o type.o number.o reader.o symbol.o eval.o boolean.o list.o util.o io.o

test.o: test.c shoelaces.h

type.o: type.c shoelaces.h

reader.o: reader.c shoelaces.h

number.o: number.c shoelaces.h

symbol.o: symbol.c shoelaces.h

eval.o: eval.c shoelaces.h

boolean.o: boolean.c shoelaces.h

list.o: list.c shoelaces.h

util.o: util.c shoelaces.h

io.o: io.c shoelaces.h

shoelaces.h: khash.h

clean:
	rm -rf *.o *.dSYM shoelaces test

.PHONY: clean
