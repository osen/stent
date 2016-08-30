include common.Mk

all: bin lib bin/emptest

bin/emptest: $(EMPTEST_SRC) $(EMPTEST_INC) lib/libstent.a
	gcc -o bin/emptest -Isrc/stent $(EMPTEST_SRC) lib/libstent.a

lib/libstent.a: $(LIBSTENT_SRC) $(LIBSTENT_INC)
	-rm -f *.o
	gcc -c $(LIBSTENT_SRC)
	ar rcs lib/libstent.a *.o
	-rm -f *.o

bin:
	mkdir bin

lib:
	mkdir lib

clean:
	rm -f *.o
	rm -r -f bin
	rm -r -f lib
