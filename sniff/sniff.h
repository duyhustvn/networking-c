#include <stdlib.h>

void callback(u_char *useless, const struct pcap_pkthdr *pkthdr,
              const u_char *packet);
void sniff(char *dev, char *protocol, int num_captured_packets);
