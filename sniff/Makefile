# Compiler flags
CC = gcc
CFLAGS = -Wall

# Project files
ODIR = obj
LIBS = -lpcap
EXE = main.out

OBJS = sniff.o demo.o utils.o ethernet.o tcp.o dns.o udp.o

SRC_DIR = src
#
# Debug build settings
#
BUILD_DIR = build
BUILD_EXE = $(BUILD_DIR)/$(EXE)
BUILD_OBJS = $(addprefix $(BUILD_DIR)/, $(OBJS))
DBGCFLAGS = -g

#.PHONY: debug clean
#
# Debug rules
#

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

build: $(BUILD_EXE)
$(BUILD_EXE): $(BUILD_OBJS)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(BUILD_EXE) $^ $(LIBS)

run:
	$(BUILD_EXE) 1 tcp 10000

clean:
	rm -rf $(EXE) $(ODIR) $(BUILD_DIR)
