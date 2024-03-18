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

bool extractFlagNs(unsigned char *tcpHeaderPtr) {
  // 12th byte bit 0
  return *(tcpHeaderPtr + 12) & 0x01;
};

bool extractFlagCwr(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 7
  // 0x80 = 1000.0000
  return *(tcpHeaderPtr + 13) & 0x80;
};

bool extractFlagEce(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 6
  // 0x40 = 0100.0000
  return *(tcpHeaderPtr + 13) & 0x40;
};

bool extractFlagUrg(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 5
  // 0x20 = 0010.0000
  return *(tcpHeaderPtr + 13) & 0x20;
};

bool extractFlagAck(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 4
  // 0x10 = 0001.0000
  return *(tcpHeaderPtr + 13) & 0x10;
};

bool extractFlagPsh(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 3
  // 0x08 = 0000.1000
  return *(tcpHeaderPtr + 13) & 0x08;
};

bool extractFlagRst(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 2
  // 0x08 = 0000.0100
  return *(tcpHeaderPtr + 13) & 0x04;
};

bool extractFlagSync(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 1
  // 0x08 = 0000.0010
  return *(tcpHeaderPtr + 13) & 0x02;
};

bool extractFlagFin(unsigned char *tcpHeaderPtr) {
  // 13th byte bit 0
  // 0x08 = 0000.0001
  return *(tcpHeaderPtr + 13) & 0x01;
};

unsigned int extractWindowSize(unsigned char *tcpHeaderPtr) {
  return conv2BytesToInt(tcpHeaderPtr + 14);
};

unsigned int extractChecksum(unsigned char *tcpHeaderPtr) {
  return conv2BytesToInt(tcpHeaderPtr + 16);
};

unsigned int extractUrgentPointer(unsigned char *tcpHeaderPtr) {
  return conv2BytesToInt(tcpHeaderPtr + 18);
};

void printTcpHeader(struct tcpHeader *self) {
  printf("---- TCP HEADER ----\n\n");
  printf("%20s | %u\n", "Source Port", self->srcPort);
  printf("%20s | %u\n", "Destination Port", self->dstPort);
  printf("%20s | %u\n", "Sequence Number", self->seqNumber);
  printf("%20s | %u\n", "Ack Number", self->ackNumber);
  printf("%20s | %u\n", "Offset", self->offset);
  printf("%20s | %u\n", "Reserved", self->reserved);

  // Flags
  printf("%20s | %u\n", "Flags", 9);
  printf("%20s : %u\n", "ns", self->flags.ns);
  printf("%20s : %u\n", "cwr", self->flags.cwr);
  printf("%20s : %u\n", "ece", self->flags.ece);
  printf("%20s : %u\n", "urg", self->flags.urg);
  printf("%20s : %u\n", "ack", self->flags.ack);
  printf("%20s : %u\n", "psh", self->flags.psh);
  printf("%20s : %u\n", "rst", self->flags.rst);
  printf("%20s : %u\n", "sync", self->flags.sync);
  printf("%20s : %u\n", "fin", self->flags.fin);

  printf("%20s | %u\n", "Window size", self->windown);
  printf("%20s | %u\n", "Check sum", self->checksum);
  printf("%20s | %u\n", "Urgent pointer", self->urgentPointer);
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

  t->windown = extractWindowSize(tcpHeaderPtr);
  t->checksum = extractChecksum(tcpHeaderPtr);
  t->urgentPointer = extractUrgentPointer(tcpHeaderPtr);

  // methos
  t->printTcpHeader = printTcpHeader;
  return t;
}
