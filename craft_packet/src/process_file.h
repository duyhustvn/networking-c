#ifndef PROCESS_FILE_H_
#define PROCESS_FILE_H_

#include <libnet.h>

int readAndProcessFileByChunk(libnet_t* l, char *fileName, char *srcIP, char *srcMac, char *dstMac);

#endif // PROCESS_FILE_H_
