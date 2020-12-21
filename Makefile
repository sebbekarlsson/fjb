exec = fjb.out
sources = $(wildcard src/*.c)
objects = $(sources:.c=.o)
flags = -I./external/libjson/src/include -g -Wall -lm -ldl -fPIC -rdynamic -L./ -ljson 

$(exec): $(objects) libjson.a
	gcc $(objects) $(flags) -o $(exec)

libfjb.a: $(objects)
	ar rcs $@ $^

%.o: %.c include/%.h
	gcc -c $(flags) $< -o $@

libjson.a:
	cd external/libjson ; make ; mv ./libjson.a ../../.

install:
	make
	make libfjb.a
	mkdir -p /usr/local/include/fjb
	cp -r ./src/include/* /usr/local/include/fjb/.
	cp ./libfjb.a /usr/local/lib/.
	cp ./fjb.out /usr/local/bin/fjb

clean:
	-rm *.out
	-rm *.o
	-rm *.a
	-rm src/*.o

lint:
	clang-tidy src/*.c src/include/*.h
