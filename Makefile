main: *.c src/*.c src/*.h
	gcc main.c src/pabort.c src/lis3dh-spi-dev.c src/lis3dh.c -o main

clean:
	rm -f main
