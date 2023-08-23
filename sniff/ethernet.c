#include "ethernet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printMacAddr(const unsigned char *data) {
  for (int i = 0; i < 6; i++) {
    printf("%.2x", data[i]);
    if (i < 5) {
      printf(".");
    }
  }
  printf("\n");
}

void printEtherFrame(struct ethernetFrame *self) {
  printf("---- ETHERNET HEADER ----\n");
  printf("%20s |", "Source Mac");
  printMacAddr(self->srcMac);
  printf("%20s |", "Dest Mac");
  printMacAddr(self->dstMac);
}

struct ethernetFrame *extractEthernetFrame(unsigned char *ethernetFramePtr) {
  struct ethernetFrame *e =
      (struct ethernetFrame *)malloc(sizeof(struct ethernetFrame));

  memcpy(e->dstMac, ethernetFramePtr, 6);
  memcpy(e->srcMac, ethernetFramePtr + 6, 6);

  e->printEtherFrame = printEtherFrame;
  return e;
}
