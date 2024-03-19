#ifndef PROCESS_FILE_H_
#define PROCESS_FILE_H_

#include <libnet.h>

int readAndProcessFileByChunk(char* fileName, libnet_t* l, char* srcIP, char* srcMac);
void processChunk(char* chunk, libnet_t* l, char* srcIP, char* srcMac);

#endif // PROCESS_FILE_H_
