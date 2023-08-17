OBJECTS = list_device.o demo.o

main.out: ${OBJECTS}
			cc ${OBJECTS} -o main.out -lpcap

list_device.o: list_device.c list_device.h
			cc -c list_device.c

demo.o: list_device.h demo.c
		cc -c demo.c
