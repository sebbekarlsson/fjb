exec = fjb.out
sources = $(wildcard src/*.c) $(wildcard src/hooks/*.c)
objects = $(sources:.c=.o)
flags = -I$$HOME/.local/include -lm -ldl -fPIC -rdynamic -L$$HOME/.local/lib -lhashmap -ljson -lscss -lpthread

objects_no_main = $(filter-out src/main.o, $(objects))

jsfiles = $(wildcard src/js/*.js)
jsheaders = $(jsfiles:.js=.js.h)

gppfiles = $(wildcard src/enums/*.gpp)
gppheaders = $(gppfiles:.gpp=.h)

GPP_PATH=$(or $(shell test -f ../gpp.out && echo ../gpp.out), $(or $(shell test -f ./gpp.out && echo ./gpp.out), gpp))

ifdef DEBUG
flags += -D DEBUG -pg -Wall -g
endif


$(exec): $(jsheaders) $(gppheaders) $(objects) version.h libjson.a libhashmap.a libscss.a
	gcc $(objects) $(flags) -g -o $(exec)

libfjb.a: $(objects_no_main)
	ar rcs $@ $^

%.o: %.c include/%.h
	gcc -c $(flags) $< -o $@

%.o: %.c ../include/hooks/%.h
	gcc -c $(flags) $< -o $@

%.js.h: %.js
	mkdir -p ./src/include/js
	mkdir -p .tmp
	$(GPP_PATH) $^ | tr -d \\n | sed -e :a -e '/[^[:blank:]]/,$$!d; /^[[:space:]]*$$/{ $$d; N; ba' -e '}' > .tmp/$(notdir $^)
	xxd -i .tmp/$(notdir $^) | sed 's/\([0-9a-f]\)$$/\0, 0x00/' > src/include/js/$(notdir $^.h)

%.h: %.gpp
	mkdir -p ./src/include/enums
	$(GPP_PATH) $^ > ./src/include/enums/$(notdir $^.h)
	#xxd -i .tmp/$(notdir $^) > src/include/js/$(notdir $^.h)
 
libjson.a:
	cd external/libjson ; make clean ; make ; make install; mv ./libjson.a ../../.

libscss.a:
	cd external/scss ; make clean ; make ; make install; mv ./libscss.a ../../.

liblist.a:
	cd external/liblist ; make clean ; make ; make install ; mv *.a ../../.

libhashmap.a:
	cd external/hashmap ; make clean ; make ; make install ; mv ./libhashmap.a ../../.

version.h:
	mkdir -p .tmp
	./version.sh > .tmp/$@
	xxd -i .tmp/$@ | sed 's/\([0-9a-f]\)$$/\0, 0x00/' > src/include/$@

install:
	make
	make libfjb.a
	mkdir -p $(HOME)/.local/include/fjb
	cp -r ./src/include/* $(HOME)/.local/include/fjb/.
	cp ./libfjb.a $(HOME)/.local/lib/.
	cp ./fjb.out $(HOME)/.local/bin/fjb

clean:
	-rm *.out
	-rm *.o
	-rm *.a
	-rm src/*.o
	-rm src/hooks/*.o
	-rm -rf .tmp

lint:
	clang-tidy src/*.c src/include/*.h
