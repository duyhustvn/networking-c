#include <errno.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <pcap.h>
#include <pcap/pcap.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <arpa/inet.h> // struct in_addr

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

// callback of pcap_loop for processing captured packet
void callback(u_char *useless, const struct pcap_pkthdr *pkthdr,
              const u_char *packet) {
  static int count = 1;
  printf("Packet number [%d], length of this packet is: %d bytes (%d%%) of "
         "packet\n",
         count++, pkthdr->len, (int)(100.0 * pkthdr->caplen / pkthdr->len));

  uint16_t etherType = determinePacketType(packet);
  printPacketType(etherType);
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
