# Compiler flags
CC = gcc
CFLAGS = -Wall

# Project files
ODIR = obj
LIBS = -lpcap
EXE = main.out

OBJS = sniff.o demo.o utils.o ethernet.o tcp.o dns.o udp.o

#
# Debug build settings
#
DBGDIR = debug
DBGEXE = $(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGCFLAGS = -g

.PHONY: debug clean
#
# Debug rules
#
debug: $(DBGEXE)
$(DBGEXE): $(DBGOBJS)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(DBGEXE) $^ $(LIBS)
$(DBGDIR)/%.o: %.c
	mkdir -p $(DBGDIR)
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

clean:
	rm -rf $(EXE) $(ODIR) $(DBGDIR)
