main: *.c *.h src/*.c src/*.h
	gcc main.c args.c src/pabort.c src/lis3dh-spi-dev.c src/lis3dh.c -o main -lwiringPi

clean:
	rm -f main
