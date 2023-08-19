#include <pcap.h>
#include <stdlib.h>

void callback(u_char *useless, const struct pcap_pkthdr *pkthdr,
              const u_char *packet);
void sniff(char *dev, char *protocol, int num_captured_packets);
char *convertIpFromNumberToText(bpf_u_int32 ip);
uint16_t determinePacketType(const u_char *packet);
void printPacketType(uint16_t etherType);
