all:
	gcc -Wall client.c -o client -pthread -lrt
	gcc barber.c -o barber -pthread -lrt
	gcc hairdresser.c -o hairdresser -pthread -lrt

clean:
	rm -rf *.o client barber hairdresser
