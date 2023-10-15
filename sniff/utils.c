#include "utils.h"

#include <stdio.h>
#include <sys/types.h>

void printHex(const u_char *ptr, int len) {
  for (int i = 0; i < len; i++) {
    printf("%02X ", ptr[i]);
  }
  printf("\n");
}

unsigned int conv2BytesToInt(const unsigned char *p) {
  unsigned int i = p[0];
  return i << 8 | p[1];
}

unsigned int conv4BytesToInt(const unsigned char *p) {
  unsigned int res = p[0];

  res = res << 8 | p[1];
  res = res << 8 | p[2];
  res = res << 8 | p[3];

  return res;
};
