#ifndef ETHERNET_H_
#define ETHERNET_H_

/* Header lengths in bytes
 ** The ethernet header length is always 14 bytes
 ** 6 bytes for Dest Mac, 6 bytes for Source Mac and 2 bytes for Ether Type
 */
struct ethernetFrame {
  unsigned char dstMac[6]; // 6 byte destination mac address
  unsigned char srcMac[6]; // 6 byte source mac address
  unsigned char typ[2];    // 2 byte ethernet type

  // methods
  void (*printEtherFrame)(struct ethernetFrame *self);
};

struct ethernetFrame *extractEthernetFrame(unsigned char *ethernetFramePtr);
void printMacAddr(const unsigned char *data);

#endif // ETHERNET_H_
