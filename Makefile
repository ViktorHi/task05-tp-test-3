# Makefile for Autopark project

CFLAGS = -I. -Wall -ftest-coverage -fprofile-arcs 

autopark: main.o db_routines.o ui.o car_tester.o sum_counter.o test
	gcc -o bin/autopark obj/*.o -lsqlite3

main.o: src/main.c
	gcc -c -std=c99 -o obj/main.o src/main.c

db_routines.o: src/db_routines.c
	gcc -c -std=c99 -o obj/db_routines.o src/db_routines.c

car_tester.o: src/car_tester.c
	gcc -c -std=c99 -o obj/car_tester.o src/car_tester.c

sum_counter.o: src/sum_counter.c
	gcc -c -std=c99 -o obj/sum_counter.o src/sum_counter.c

ui.o: src/ui.c
	gcc -c -std=c99 -o obj/ui.o src/ui.c

test: test/test-library.c 
	gcc -c test/test-library.c -o obj/test-library.o
	gcc -o test-library.out obj/test-library.o obj/sum_counter.o $(CFLAGS) -lm -lncurses -Os -lsqlite3
	rm obj/test-library.o

clean:
	rm -f obj/*.o bin/*
