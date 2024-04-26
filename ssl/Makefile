#  $@ The name of the target file (the one before the colon)
#  $< The name of the first (or only) prerequisite file (the first one after the colon)
#  $^ The names of all the prerequisite files (space-separated)
#  $* The stem (the bit which matches the `%` wildcard in the rule definition.
#

# Compiler flags
CC = gcc
CFLAGS = -Wall

# Project files
ODIR = obj
LIBS = -lcrypto
EXE = main.out

OBJS = decryption.o

SRC_DIR = src

BUILD_DIR = build
BUILD_EXE = $(BUILD_DIR)/$(EXE)
BUILD_OBJS = $(addprefix $(BUILD_DIR)/, $(OBJS))
DBGCFLAGS = -g

build: $(BUILD_EXE)
$(BUILD_EXE): $(BUILD_OBJS)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(BUILD_EXE) $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $< $(LIBS)

run:
	FILE=tls_files/admin.crt $(BUILD_EXE)

clean:
	rm -rf $(BUILD_DIR)
