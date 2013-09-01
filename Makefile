CC=clang
CFLAGS=-g -Wall -std=c99

shoestrap: shoestrap.o type.o number.o reader.o symbol.o eval.o boolean.o list.o util.o io.o string.o gc.o func.o

run-tests: test
	./test

.PHONY: run-tests

test: test.o type.o number.o reader.o symbol.o eval.o boolean.o list.o util.o io.o string.o gc.o func.o

shoestrap.o: shoestrap.c shoelaces.h

test.o: test.c shoelaces.h

type.o: type.c shoelaces.h internal.h

reader.o: reader.c shoelaces.h internal.h

number.o: number.c shoelaces.h internal.h

symbol.o: symbol.c shoelaces.h internal.h

eval.o: eval.c shoelaces.h internal.h

boolean.o: boolean.c shoelaces.h internal.h

list.o: list.c shoelaces.h internal.h

util.o: util.c shoelaces.h internal.h

io.o: io.c shoelaces.h internal.h

string.o: string.c shoelaces.h internal.h

gc.o: gc.c shoelaces.h internal.h

func.o: func.c shoelaces.h internal.h

shoelaces.h: khash.h

clean:
	rm -rf *.o *.dSYM shoestrap test

.PHONY: clean
