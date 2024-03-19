#ifndef CRAFT_TCP_H_
#define CRAFT_TCP_H_

#include <stdio.h>
#include <stdint.h>

#include <libnet.h>

int craftTcpPacket(libnet_t* l, uint16_t srcPort, uint16_t dstPort, uint32_t seq, uint32_t ack, uint8_t control, char* srcIP, char* dstIP, char* srcMac, char* dstMac, char* errstr);

#endif // CRAFT_TCP_H_
