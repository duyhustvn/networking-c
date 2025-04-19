# Compiler flags
CC = gcc
CFLAGS = -Wall

# Project files
LIBS =
EXE = sniff_packet

BUILD_DIR = build
BUILD_EXE = $(BUILD_DIR)/$(EXE)

run:
	DEVICE_INTERFACE=wlp1s0  $(BUILD_EXE) ring 

clean:
	rm -rf $(EXE) $(ODIR) $(BUILD_DIR)
