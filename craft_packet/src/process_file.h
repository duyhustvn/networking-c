#ifndef PROCESS_FILE_H_
#define PROCESS_FILE_H_

#include <libnet.h>

#include "config.h"
#include "data_circle_linked_list.h"

int readAndProcessFileByChunk(config cfg);
void processChunk(IPCircleLinkedList **lls,  int numThreads, char* chunk, uint32_t srcIP, uint8_t* srcMac, uint8_t* dstMac);

#endif // PROCESS_FILE_H_
