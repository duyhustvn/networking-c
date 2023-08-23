#include "tcp.h"
#include "utils.h"
#include <stdio.h>

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
  return (*(tcpHeaderPtr + 12) >> 1) & 0x07;
};

int extractFlagNs(unsigned char *tcpHeaderPtr) {
  // 12th byte bit 0
  return *(tcpHeaderPtr + 12) & 0x01;
};
int extractFlagCwr(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 7
  // 0x80 = 1000.0000
  return *(tcpHeaderPtr + 13) & 0x80;
};

int extractFlagEce(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 6
  // 0x40 = 0100.0000
  return *(tcpHeaderPtr + 13) & 0x40;
};
int extractFlagUrg(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 5
  // 0x20 = 0010.0000
  return *(tcpHeaderPtr + 13) & 0x20;
};
int extractFlagAck(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 4
  // 0x10 = 0001.0000
  return *(tcpHeaderPtr + 13) & 0x10;
};
int extractFlagPsh(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 3
  // 0x08 = 0000.1000
  return *(tcpHeaderPtr + 13) & 0x08;
};

int extractFlagRst(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 2
  // 0x08 = 0000.0100
  return *(tcpHeaderPtr + 13) & 0x04;
};
int extractFlagSync(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 1
  // 0x08 = 0000.0010
  return *(tcpHeaderPtr + 13) & 0x02;
};
int extractFlagFin(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 0
  // 0x08 = 0000.0001
  return *(tcpHeaderPtr + 13) & 0x01;
};

void printTcpHeader(struct tcpHeader *self) {
  printf("Source Port: %u\n", self->srcPort);
  printf("Destination Port: %u\n", self->dstPort);
  printf("Sequence Number: %u\n", self->seqNumber);
  printf("Ack Number: %u\n", self->ackNumber);
  printf("Offset: %u -> tcp header length: %u\n", self->offset,
         4 * self->offset);
  printf("Reserved: %u\n", self->reserved);
  // Flags
  printf("Flag ns: %u\n", self->flags.ns);
  printf("Flag cwr: %u\n", self->flags.cwr);
  printf("Flag ece: %u\n", self->flags.ece);
  printf("Flag urg: %u\n", self->flags.urg);
  printf("Flag ack: %u\n", self->flags.ack);
  printf("Flag psh: %u\n", self->flags.psh);
  printf("Flag rst: %u\n", self->flags.rst);
  printf("Flag sync: %u\n", self->flags.sync);
  printf("Flag fin: %u\n", self->flags.fin);
};

struct tcpHeader *extractTcpHeader(unsigned char *tcpHeaderPtr) {
  struct tcpHeader *t = (struct tcpHeader *)malloc(sizeof(struct tcpHeader));

  t->srcPort = extractSrcPort(tcpHeaderPtr);
  t->dstPort = extractDstPort(tcpHeaderPtr);
  t->seqNumber = extractSequenceNumber(tcpHeaderPtr);
  t->ackNumber = extractAckNumber(tcpHeaderPtr);
  t->offset = extractOffset(tcpHeaderPtr);
  t->reserved = extractReserved(tcpHeaderPtr);
  // flags
  t->flags.ns = extractFlagNs(tcpHeaderPtr);
  t->flags.cwr = extractFlagCwr(tcpHeaderPtr);
  t->flags.ece = extractFlagEce(tcpHeaderPtr);
  t->flags.urg = extractFlagUrg(tcpHeaderPtr);
  t->flags.ack = extractFlagAck(tcpHeaderPtr);
  t->flags.psh = extractFlagPsh(tcpHeaderPtr);
  t->flags.rst = extractFlagRst(tcpHeaderPtr);
  t->flags.sync = extractFlagSync(tcpHeaderPtr);
  t->flags.fin = extractFlagFin(tcpHeaderPtr);

  // methos
  t->printTcpHeader = printTcpHeader;
  return t;
}
