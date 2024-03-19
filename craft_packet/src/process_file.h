#ifndef PROCESS_FILE_H_
#define PROCESS_FILE_H_

#include <libnet.h>

int readAndProcessFileByChunk(char* fileName, libnet_t* l);
void processChunk(char* chunk, libnet_t* l);

#endif // PROCESS_FILE_H_
