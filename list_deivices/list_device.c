// references:
// https://www.thegeekstuff.com/2012/10/packet-sniffing-using-libpcap/
#include <pcap/pcap.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <pcap.h>

void listDevices() {
  char *dev;
  char errbuf[PCAP_ERRBUF_SIZE];

  pcap_t *descr;

  struct bpf_program fp; // to hold compiled program
  bpf_u_int32 pMask;     // subnet mask
  bpf_u_int32 pNet;      // ip address

  pcap_if_t *alldevs, *d;
  char dev_buff[64] = {0};
  int countDev = 0; // count number of devices

  // prepare a list of all devices
  if (pcap_findalldevs(&alldevs, errbuf) == -1) {
    fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
    exit(1);
  }

  // print the list of all devices
  printf("\nHere is a list of available devices on your system:\n\n");
  for (d = alldevs; d; d = d->next) {
    printf("%d. %s", ++countDev, d->name);
    if (d->description) {
      printf("(%s)\n", d->description);
    } else {
      printf("Sorry. No description available for this device\n");
    }
  }
}
