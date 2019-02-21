all: bin cmake bin/emptest

cmake: build build/stage_done
	cd build && cmake --build .

bin/emptest: build/emptest
	cp build/emptest bin/emptest

build/stage_done:
	cd build && cmake ../src
	touch build/stage_done

build:
	mkdir build

bin:
	mkdir bin

clean:
	rm -r -f build
	rm -r -f bin
