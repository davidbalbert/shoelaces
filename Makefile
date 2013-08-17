CC=clang
CFLAGS=-g

shoelaces: shoelaces.c

run-tests: test
	./test

test: test.o type.o number.o reader.o symbol.o eval.o

test.o: test.c shoelaces.h

type.o: type.c shoelaces.h

reader.o: reader.c shoelaces.h

number.o: number.c shoelaces.h

symbol.o: symbol.c shoelaces.h

eval.o: eval.c shoelaces.h

shoelaces.h: khash.h

clean:
	rm -rf *.o *.dSYM shoelaces test

.PHONY: clean
