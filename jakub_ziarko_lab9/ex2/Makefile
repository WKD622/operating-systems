all:
	gcc -Wall main.c -o main -pthread -lrt
	gcc -Wall customer.c -o customer -pthread -lrt
	gcc -Wall manufacturer.c -o manufacturer -pthread -lrt
clean:
	rm -rf *.o main manufacturer customer
