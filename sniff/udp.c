#include <stdio.h>
#include <stdlib.h>

#include "udp.h"
#include "utils.h"

void printUdpHeader(struct udpHeader *self) {
  // printf("%-30s%-30s%-30s%-30s\n", "Source Port", "Dest Port", "Length",
  //        "Checksum");

  // printf("%-30d%-30d%-30d%-30d\n", self->srcPort, self->dstPort, self->len,
  //        self->sum);
}

struct udpHeader *extractUdpHeader(const u_char *udpHeaderPtr) {
  struct udpHeader *extractedUdpHeader =
      (struct udpHeader *)malloc(sizeof(struct udpHeader));

  extractedUdpHeader->srcPort = conv2BytesToInt(udpHeaderPtr);
  extractedUdpHeader->dstPort = conv2BytesToInt(udpHeaderPtr + 2);
  extractedUdpHeader->len = conv2BytesToInt(udpHeaderPtr + 4);
  extractedUdpHeader->sum = conv2BytesToInt(udpHeaderPtr + 6);
  return extractedUdpHeader;
}
