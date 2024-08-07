#  $@ The name of the target file (the one before the colon)
#  $< The name of the first (or only) prerequisite file (the first one after the colon)
#  $^ The names of all the prerequisite files (space-separated)
#  $* The stem (the bit which matches the `%` wildcard in the rule definition.
# Compiler flags
# CC = gcc
CC = clang
CFLAGS = -Wall

ifdef DEBUG
	CFLAGS += -DDEBUG
endif

# Project files
LIBS = -lpcap -lnet -lpthread
EXE = smurf_ip

OBJS = log.o craft_tcp.o process_file.o data_queue.o data_circle_linked_list.o thread_process.o util_signal.o util_cpu.o main.o

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


$(BUILD_DIR)/%.o: $(SRC_DIR)/*/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $< $(LIBS)

run:
	# FILE="./statics/sample" SOURCE_IP="192.168.114.43" SOURCE_MAC="28:df:eb:6a:8c:02" DEST_MAC="e0:23:ff:e9:53:9f" DEVICE_INTERFACE="wlp0s20f3" $(BUILD_EXE)
	# $(BUILD_EXE) -a 192.168.114.43 -b 28:df:eb:6a:8c:02 -c e0:23:ff:e9:53:9f -d statics/sample -e wlp0s20f3 -f 20
	$(BUILD_EXE) -a 192.168.101.8 -b c8:09:a8:4f:19:7f -c 3c:ff:d8:c1:d9:71 -d statics/sample -e wlp0s20f3 -f 1


clean:
	rm -rf $(BUILD_DIR)

test_queue: ${BUILD_DIR}/data_queue.o src/data_queue_test.c
	gcc -o data_queue_test.o $^ -lcmocka -lm
	./data_queue_test.o
	rm ./data_queue_test.o

test_circle_linked_list: ${BUILD_DIR}/data_circle_linked_list.o src/data_circle_linked_list_test.c
	gcc -o data_circle_linked_list_test.o $^ -lcmocka -lm
	./data_circle_linked_list_test.o
	rm ./data_circle_linked_list_test.o

valgrind-check:
	valgrind --leak-check=yes --track-origins=yes ${BUILD_EXE} -a 192.168.101.3 -b 28:df:eb:6a:8c:01 -c 3c:ff:d8:c1:d9:70 -d statics/sample -e wlp0s20f3
