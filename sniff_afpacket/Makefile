# Compiler flags
CC = gcc
CFLAGS = -Wall

# Project files
ODIR = obj
LIBS =
EXE = main.out

OBJS = main.o

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
	DEVICE_INTERFACE=wlp0s20f3 $(BUILD_EXE) 1 tcp 10000

clean:
	rm -rf $(EXE) $(ODIR) $(BUILD_DIR)
