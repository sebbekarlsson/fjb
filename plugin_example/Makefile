sources = $(wildcard *.c)
objects = $(sources:.c=.o)
flags = -I$$HOME/.local/include -g -Wall -lm -ldl -fPIC -rdynamic -L$$HOME/.local/lib -lfjb
LPATH = $(HOME)/.local


libmyplugin.so: $(objects)
	gcc $(flags) -o $@ $^ -shared

%.o: %.c
	gcc -c $(flags) $< -o $@

install:
	make
	make libmyplugin.a
	mkdir -p $(LPATH)/include/myplugin
	cp -r ./src/include/* $(LPATH)/include/myplugin/.
	cp ./libmyplugin.a $(LPATH)/lib/.
	cp ./myplugin.out $(LPATH)/bin/myplugin

clean:
	-rm *.out
	-rm *.o
	-rm *.a
	-rm src/*.o

lint:
	clang-tidy src/*.c src/include/*.h
