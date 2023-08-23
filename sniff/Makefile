OBJECTS = sniff.o demo.o tcp.o utils.c

main.out: ${OBJECTS}
		cc ${OBJECTS} -o main.out -lpcap

sniff.o: sniff.c sniff.h
		cc -c sniff.c

demo.o: sniff.h demo.c
		cc -c demo.c

tcp.o: tcp.h tcp.c
	cc -c tcp.c

utils.o: utils.h utils.c
	cc -c utils.c
