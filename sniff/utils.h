#ifndef UTILS_H_
#define UTILS_H_

#include <sys/types.h>

void printHex(const u_char *ptr, int len);

// Convert 2 byte of data to unsigned int
unsigned int conv2BytesToInt(const unsigned char *p);

// Convert 4 byte of data to unsigned int
unsigned int conv4BytesToInt(const unsigned char *p);

#endif // UTILS_H_
