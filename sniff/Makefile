OBJECTS = sniff.o demo.o

main.out: ${OBJECTS}
		cc ${OBJECTS} -o main.out -lpcap

sniff.o: sniff.c sniff.h
		cc -c sniff.c

demo.o: sniff.h demo.c
		cc -c demo.c
