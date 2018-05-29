#STATIC 
static: main.o libzad1.o
	gcc main.o libzad1.o -o main

main.o: main.c
	gcc -D STATIC_LIB -c main.c

libzad1.o: libzad1.c
	gcc -c libzad1.c

#SHARED
shared: libzad1sh.o intoShared
	gcc -L. -D SHARED_LIB -o main main.c -lzad1 -Wl,-rpath=.

libzad1sh.o: libzad1.c
	gcc -c -fpic libzad1.c

intoShared: libzad1.o
	gcc -shared -o libzad1.so libzad1.o


#DYNAMIC
dynamic:
	gcc -c -fpic libzad1.c
	gcc -shared -o libzad1.so libzad1.o
	gcc -D DYNAMIC_LIB main.c -o main -ldl

#CLEAN
clean:
	rm -rf *o main *.txt

smallclean:
	rm -rf *o main

3a:
	> results3a.txt
	gcc -D STATIC_LIB -c main.c
	gcc -c libzad1.c
	gcc main.o libzad1.o -o main
	./main >> results3a.txt
	gcc -c -fpic libzad1.c
	gcc -shared -o libzad1.so libzad1.o
	gcc -L. -D SHARED_LIB -o main main.c -lzad1 -Wl,-rpath=.
	./main >> results3a.txt
	gcc -c -fpic libzad1.c
	gcc -shared -o libzad1.so libzad1.o
	gcc -D DYNAMIC_LIB main.c -o main -ldl
	./main >> results3a.txt

3b:
	> results3b.txt
	gcc -D STATIC_LIB -c main.c -o main.o
	gcc -c libzad1.c
	gcc main.o libzad1.o -o main
	./main >> results3b.txt
	gcc -D STATIC_LIB -c main.c -o2 main.o
	gcc -c libzad1.c
	gcc main.o libzad1.o -o2 main
	./main >> results3b.txt
	gcc -D STATIC_LIB -c main.c -o3 main.o
	gcc -c libzad1.c
	gcc main.o libzad1.o -o3 main
	./main >> results3b.txt
