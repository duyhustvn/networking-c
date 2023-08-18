#include <errno.h>
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void callback(u_char *useless, const struct pcap_pkthdr *pkthdr,
              const u_char *packet) {
  static int count = 1;
  printf("\nPacket number [%d], length of this packet is: %d\n", count++,
         pkthdr->len);
}

void sniff(char *dev, char *protocol, int num_captured_packets) {
  bpf_u_int32 pMask; // subnet mask
  bpf_u_int32 pNet;  // ip address

  struct bpf_program fp; // to hold compiled program
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t *descr;

  // fetch the network address and network mask
  pcap_lookupnet(dev, &pNet, &pMask, errbuf);

  // Now open device for sniffing
  descr = pcap_open_live(dev, BUFSIZ, 0, -1, errbuf);
  if (descr == NULL) {
    printf("pcap_open_live() failed due to [%s]\n", errbuf);
    return;
  }

  // Compile filter expression
  if (pcap_compile(descr, &fp, protocol, 0, pNet) == -1) {
    printf("\npcap_compile() failed\n");
    return;
  }

  // set the filter compiled above
  if (pcap_setfilter(descr, &fp) == -1) {
    printf("\npcap_setfilter() failed\n");
    return;
  }

  // for every packet received, call the callback function
  // for now, maximum limit on number of packets is specified
  // by user
  //
  pcap_loop(descr, num_captured_packets, callback, NULL);

  printf("\nDone with packet sniffing\n");
}
