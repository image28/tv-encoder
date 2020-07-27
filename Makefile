all: decode encode 
encode:encode.c
	gcc encode.c -g -o encode `sdl2-config --libs --cflags` -lpng -lSDL2_image
decode:decode.c
	gcc decode.c -g -o decode `sdl2-config --libs --cflags` -lpng -lSDL2_image
test-example:test-example.c
	gcc test-example.c -Ofast -g -o test-example `sdl2-config --libs --cflags` -lpng -lSDL2_image
clean:
	rm -f ./*~ && rm -f ./{decode,encode,test-example}
