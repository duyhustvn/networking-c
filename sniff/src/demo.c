#include "sniff.h"
#include <pcap/pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  char *dev;
  char dev_buff[64] = {0};
  char errbuf[PCAP_ERRBUF_SIZE];
  char *protocol;
  int filter;
  int num_captured_packets;

  if (argc != 4) {
    printf("Usage: [%s] [filter] [protocol] [number-of-packets]\n", argv[0]);
    return 0;
  }

  filter = (atoi)(argv[1]);
  protocol = argv[2];
  num_captured_packets = (atoi)(argv[3]);

  dev = "wlp0s20f3"; // default
  // Ask user to provide the interface name
  printf("Enter the interface name on which you wanna run the packet "
         "sniffer <default: wlp0s20f3>: ");
  fgets(dev_buff, sizeof(dev_buff) - 1, stdin);

  // clear off the trailing newline that fgets sets
  dev_buff[strlen(dev_buff) - 1] = '\0';

  // check if something was provided by user
  if (strlen(dev_buff) != 0) {
    dev = dev_buff;
  }

  printf("\n -- You opted for device [%s] to capture [%d] packets --\n", dev,
         (atoi)(argv[2]));

  // if smth was not provided
  // return error
  if (dev == NULL) {
    printf("\n[%s]\n", errbuf);
    return -1;
  }

  // fetch the network address and network mask
  sniff(dev, filter, protocol, num_captured_packets);
}
