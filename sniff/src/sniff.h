#include <pcap.h>
#include <stdlib.h>

void callback(u_char *useless, const struct pcap_pkthdr *pkthdr,
              const u_char *packet);
void sniff(char *dev, int filter, char *protocol, int num_captured_packets);
char *convertIpFromNumberToText(bpf_u_int32 ip);
uint16_t determinePacketType(const u_char *packet);
void printPacketType(uint16_t etherType);
void printPayload(const u_char *payload, int payloadLength);
void printIPAddr(const u_char *data);
void processTcpPacket(int ethernetHeaderLength, const u_char *ipHeaderPtr,
                      int ipHeaderLength, const struct pcap_pkthdr *pkthdr,
                      const u_char *packet);
