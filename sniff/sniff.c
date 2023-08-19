#include <errno.h>
#include <netinet/in.h>
#include <pcap.h>
#include <pcap/pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void callback(u_char *useless, const struct pcap_pkthdr *pkthdr,
              const u_char *packet) {
  static int count = 1;
  printf("\nPacket number [%d], length of this packet is: %d\n", count++,
         pkthdr->len);
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
