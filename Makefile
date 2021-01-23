exec = fjb.out
sources = $(wildcard src/*.c)
objects = $(sources:.c=.o)
flags = -I./external/libjson/src/include -g -Wall -lm -ldl -fPIC -rdynamic -L./ -ljson

objects_no_main = $(filter-out src/main.o, $(objects))

jsfiles = $(wildcard src/js/*.js)
jsheaders = $(jsfiles:.js=.js.h)

GPP_PATH=$(or $(shell test -f ./gpp.out && echo ./gpp.out), gpp)

#ifdef DEBUG
	flags += -D DEBUG
#endif

$(exec): $(objects) libjson.a $(jsheaders)
	gcc $(objects) $(flags) -o $(exec)

libfjb.a: $(objects_no_main)
	ar rcs $@ $^

%.o: %.c include/%.h
	gcc -c $(flags) $< -o $@

tmp:
	mkdir -p .tmp

%.js.h: %.js
	$(GPP_PATH) $^ > .tmp/$(notdir $^)
	xxd -i .tmp/$(notdir $^) > src/include/js/$(notdir $^.h)
 
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
	-rm .tmp

lint:
	clang-tidy src/*.c src/include/*.h
