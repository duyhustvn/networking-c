#include "tcp.h"
#include "utils.h"

unsigned int extractSrcPort(unsigned char *tcpHeaderPtr) {
  return conv2BytesToInt(tcpHeaderPtr);
}

unsigned int extractDstPort(unsigned char *tcpHeaderPtr) {
  return conv2BytesToInt(tcpHeaderPtr + 2);
}

unsigned int extractSequenceNumber(unsigned char *tcpHeaderPtr) {
  return conv4BytesToInt(tcpHeaderPtr + 4);
}

unsigned int extractAckNumber(unsigned char *tcpHeaderPtr) {
  return conv4BytesToInt(tcpHeaderPtr + 8);
}

unsigned int extractOffset(unsigned char *tcpHeaderPtr) {
  return (*(tcpHeaderPtr + 12) >> 4);
}

unsigned int extractReserved(unsigned char *tcpHeaderPtr) {
  return (conv2BytesToInt(tcpHeaderPtr + 12) & 0x0FC0) >> 6;
};

struct tcpHeader *extractTcpHeader(unsigned char *tcpHeaderPtr) {
  struct tcpHeader *t = (struct tcpHeader *)malloc(sizeof(struct tcpHeader));

  t->srcPort = extractSrcPort(tcpHeaderPtr);
  t->dstPort = extractDstPort(tcpHeaderPtr);
  t->seqNumber = extractSequenceNumber(tcpHeaderPtr);
  t->ackNumber = extractAckNumber(tcpHeaderPtr);
  t->offset = extractOffset(tcpHeaderPtr);
  t->reserved = extractReserved(tcpHeaderPtr);

  return t;
}
