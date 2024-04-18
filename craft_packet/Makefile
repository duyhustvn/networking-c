# Compiler flags
CC = gcc
CFLAGS = -Wall

# Project files
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
	FILE="./statics/sample" SOURCE_IP="192.168.1.7" SOURCE_MAC="28:df:eb:6a:8c:01" DEST_MAC="c8:5a:9f:8b:47:63" DEVICE_INTERFACE="wlp0s20f3" $(BUILD_EXE)

clean:
	rm -rf $(BUILD_DIR)
