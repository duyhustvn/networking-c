#include <errno.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <pcap.h>
#include <pcap/pcap.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include <arpa/inet.h> // struct in_addr

#include "sniff.h"
#include "tcp.h"

char *convertIpFromNumberToText(bpf_u_int32 ip) {
  struct in_addr addr;
  addr.s_addr = ip;

  char *net = inet_ntoa(addr);
  if (net == NULL) {
    fprintf(stderr, "inet_ntoa convert ip from number to text failed");
    return NULL;
  }
  return net;
}

void printPacketType(uint16_t etherType) {
  switch (etherType) {
  case ETHERTYPE_IP:
    printf("IP\n");
    break;
  case ETHERTYPE_IPV6:
    printf("IPV6\n");
    break;
  case ETHERTYPE_ARP:
    printf("ARP\n");
    break;
  case ETHERTYPE_REVARP:
    printf("Reverse ARP\n");
    break;
  default:
    printf("Need more investigation\n");
  }
}

void printPayload(const u_char *payload, int payloadLength) {
  if (payloadLength > 0) {
    const u_char *tempPointer = payload;

    int byteCount = 0;
    while (byteCount++ < payloadLength) {
      printf("%c", *tempPointer);
      tempPointer++;
    }

    printf("\n");
  } else {
    printf("The payload is empty");
  }
}

void printMacAddr(const u_char *data) {
  for (int i = 0; i < 6; i++) {
    printf("%.2x", data[i]);
    if (i < 5) {
      printf(".");
    }
  }
  printf("\n");
}

void printIPAddr(const u_char *data) {
  for (int i = 0; i < 4; i++) {
    printf("%d", data[i]);
    if (i < 3) {
      printf(".");
    }
  }

  printf("\n");
}

void printTCPPort(const u_char *data, int dataLength) {
  for (int i = 0; i < dataLength; i++) {
    printf("%d", data[i]);
  }

  printf("\n");
}

// callback of pcap_loop for processing captured packet
void callback(u_char *useless, const struct pcap_pkthdr *pkthdr,
              const u_char *packet) {
  static int count = 1;

  /* The total packet length, including all headers and the data payload is
   ** stored in header->len and header->caplen. Caplen is the amount actually
   ** available,
   ** and len is the total packet length even if it is larger than what we
   ** currently have captured
   ** If the snapshot length set with pcap_open_live() is too small, you may not
   ** have the whole packet
   */
  printf("\n\nPacket number [%d] \n", count++);

  printf("Total packet available: %d bytes\n", pkthdr->caplen);
  printf("Expected packet size: %d bytes\n", pkthdr->len);

  uint16_t etherType = determinePacketType(packet);
  printPacketType(etherType);

  // Pointer to start point of headers
  const u_char *ipHeaderPtr;
  const u_char *tcpHeaderPtr;
  const u_char *payloadPtr;

  /* Header lengths in bytes
   ** The ethernet header length is always 14 bytes
   ** 6 bytes for Dest Mac, 6 bytes for Source Mac and 2 bytes for Ether Type
   ** The IP header length is stored in a 4 byte integer at byte offset 4 of the
   ** IP header
   ** The TCP header length is stored in a 4 byte integer at byte offset 12 of
   ** TCP header
   ** The payload starts at packet base location plus all the header length
   ** payload_pointer =
   ** packet_pointer + len(Ethernet header) + len(IP header) + len(TCP header)
   */
  int ethernetHeaderLength = 14; //  bytes
  int ipHeaderLength;            // in bytes
  int tcpHeaderLength;
  int payloadLength;

  u_char srcMac[6], dstMac[6];

  memcpy(dstMac, packet, 6);
  printf("Dest Mac: ");
  printMacAddr(dstMac);

  memcpy(srcMac, packet + 6, 6);
  printf("Source Mac: ");
  printMacAddr(srcMac);

  if (etherType == ETHERTYPE_IPV6) {
    // header length in ipv6 is fixed 40 bytes
    ipHeaderLength = 40;
  } else if (etherType == ETHERTYPE_IP) {
    // start of ip header
    ipHeaderPtr = packet + ethernetHeaderLength;
    // The second half of the firt byte (at byte offset 4 of the IP header)
    // contains the IP header length
    // & 0x0F is a bitwise AND operation.
    // 0x0F is a hexadecimal number that represents the binary value 00001111
    ipHeaderLength = ((*ipHeaderPtr) & 0x0F);
    // The lower 4 bits of the first byte of the IP header represent the header
    // length in 32-bit words. In other words, it tells you how many 32-bit
    // chunks are present in the header. Multiply by 4 (4 * 8 bits = 32 bits) to
    // get a byte count for pointer arithmetic For example: the first byte of
    // ipHeader = 0011.0011
    // -> ipHeaderLength = 0011.0011 & 0000.1111 = 0000.0011 = 3 (decimal)
    // -> ipLength = 3 * 32 bits = 96 bits
    // -> 96 bits / 8 = 12 bytes
    ipHeaderLength = ipHeaderLength * 4;
    printf("IP header length (IHL) in bytes: %d\n", ipHeaderLength);

    // Protocol is always the 10th byte of the IP header
    u_char protocol = *(ipHeaderPtr + 9);
    if (protocol != IPPROTO_TCP) {
      printf("Not a TCP packet. Skipping ...\n\n");
      return;
    }

    // Source IP is the 12nd byte of the IP header
    u_char srcIP[4], dstIP[4];
    memcpy(srcIP, ipHeaderPtr + 12, 4);
    printf("Source IP: ");
    printIPAddr(srcIP);

    memcpy(dstIP, ipHeaderPtr + 16, 4);
    printf("Dest IP: ");
    printIPAddr(dstIP);

    tcpHeaderPtr = packet + ethernetHeaderLength + ipHeaderLength;
    /* TCP header length is stored in the first half of the 12th byte in the TCP
     *header
     **
     ** 0xF0 = 1111.0000
     */

    tcpHeaderLength = ((*(tcpHeaderPtr + 12)) & 0xF0) >> 4;
    tcpHeaderLength = tcpHeaderLength * 4;
    printf("TCP header length in bytes: %d\n", tcpHeaderLength);

    u_char tcpHeader[tcpHeaderLength];
    memcpy(tcpHeader, tcpHeaderPtr, tcpHeaderLength);

    struct tcpHeader *extractedTcpHeader = extractTcpHeader(tcpHeader);
    extractedTcpHeader->printTcpHeader(extractedTcpHeader);

    int totalHeadersSize =
        ethernetHeaderLength + ipHeaderLength + tcpHeaderLength;
    printf("Size of all headers combined: %d\n", totalHeadersSize);

    payloadLength = pkthdr->caplen - totalHeadersSize;
    printf("Payload size: %d\n", payloadLength);
    payloadPtr = packet + payloadLength;
    printf("Memory address where payload begin: %p\n", payloadPtr);

    printPayload(payloadPtr, payloadLength);
  }

  printf("\n");
}

void sniff(char *dev, char *protocol, int num_captured_packets) {
  bpf_u_int32 pMask; // subnet mask
  bpf_u_int32 pNet;  // network address not ip address

  struct bpf_program fp; // to hold compiled program
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t *descr;

  // fetch the network address and network mask
  if (pcap_lookupnet(dev, &pNet, &pMask, errbuf) == -1) {
    fprintf(stderr, "pcap_lookupnet error %s\n", errbuf);
    return;
  };

  printf("pNet: %d\n", pNet);
  // convert ip from int to text
  char *networkIP = convertIpFromNumberToText(pNet);
  if (networkIP != NULL) {
    printf("ip addr: %s\n", networkIP);
  }

  char *subnetMask = convertIpFromNumberToText(pMask);
  if (subnetMask != NULL) {
    printf("subet mask: %s\n", subnetMask);
  }

  // Now open device for sniffing
  descr = pcap_open_live(dev, BUFSIZ, 0, -1, errbuf);
  if (descr == NULL) {
    printf("pcap_open_live() failed due to [%s]\n", errbuf);
    return;
  }

  // Compile filter expression
  if (pcap_compile(descr, &fp, protocol, 0, pNet) == -1) {
    fprintf(stderr, "pcap_compile() failed: %s \n", pcap_geterr(descr));
    return;
  }

  // set the filter compiled above
  if (pcap_setfilter(descr, &fp) == -1) {
    fprintf(stderr, "pcap_setfilter() failed: %s \n", pcap_geterr(descr));
    return;
  }

  // for every packet received, call the callback function
  // for now, maximum limit on number of packets is specified
  // by user
  //
  pcap_loop(descr, num_captured_packets, callback, NULL);

  printf("\nDone with packet sniffing\n");
}

uint16_t determinePacketType(const u_char *packet) {
  struct ether_header *etherHeader;
  /*
   * The packet is larger than the ether_header struct,
   * but we just wanna look at the first part of the packet that contains the
   * header. We force the compiler to treat the pointer to the packet as just a
   * pointer to the ether_header The data payload of the packet comes after the
   * headers. Different packet types have different header lengths though, bute
   * the ethernet header is always the same (14 bytes)
   * */

  printf("Ether type:");
  etherHeader = (struct ether_header *)packet;
  uint16_t etherType = ntohs(etherHeader->ether_type);

  return etherType;
}
