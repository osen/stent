include common.Mk

all: bin lib bin/emptest.exe

bin/emptest.exe: $(EMPTEST_SRC) $(EMPTEST_INC) lib/libstent.a
	gcc -o bin/emptest.exe -Isrc/stent $(EMPTEST_SRC) lib/libstent.a

lib/libstent.a: $(LIBSTENT_SRC) $(LIBSTENT_INC)
	-del *.o
	gcc -c $(LIBSTENT_SRC)
	ar rcs lib/libstent.a *.o
	-del *.o

bin:
	mkdir bin

lib:
	mkdir lib

clean:
	-del *.o
	-rmdir /s /q bin
	-rmdir /s /q lib
