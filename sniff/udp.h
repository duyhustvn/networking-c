#ifndef UDP_H_
#define UDP_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
** UDP header
** https://datatracker.ietf.org/doc/html/rfc768
*/
struct udpHeader {
  uint16_t srcPort; // source port (16 bit)
  uint16_t dstPort; // destination port (16 bit)
  uint16_t len;     // length (16 bit) is the length in octets
                    // including this header and the data.
                    // This means the minimum value of the length is 8
  uint16_t sum;     // check sum (16 bit)

  // method
  void (*printUdpHeader)(struct udpHeader *self);
};

struct udpHeader *extractUdpHeader(const u_char *udpHeaderPtr);

#endif // UDP_H_
