# Compiler flags
CC = gcc
CFLAGS = -Wall

# Project files
ODIR = obj
LIBS = -lpcap -lnet
EXE = main.out

OBJS = craft_tcp.o process_file.o main.o

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
build: $(BUILD_EXE)
$(BUILD_EXE): $(BUILD_OBJS)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(BUILD_EXE) $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $< $(LIBS)

run:
	SOURCE_IP="192.168.1.6" SOURCE_MAC="28:df:eb:6a:8c:01" DEST_MAC="c8:5a:9f:8b:47:61" $(BUILD_EXE)

clean:
	rm -rf $(EXE) $(ODIR) $(BUILD_DIR)
