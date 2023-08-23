OBJECTS = sniff.o demo.o utils.c ethernet.o tcp.o

main.out: ${OBJECTS}
		cc ${OBJECTS} -o main.out -lpcap

sniff.o: sniff.c sniff.h
		cc -c sniff.c

demo.o: sniff.h demo.c
		cc -c demo.c

utils.o: utils.h utils.c
	cc -c utils.c

tcp.o: tcp.h tcp.c
	cc -c tcp.c

ethernet.o: ethernet.h ethernet.c
	cc -c ethernet.c
