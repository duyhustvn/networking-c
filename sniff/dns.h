#ifndef DNS_H_
#define DNS_H_

/*
** dns format https://datatracker.ietf.org/doc/html/rfc1035#section-4
*/
#include <stdint.h>
#include <sys/types.h>

/* DNS Headers

      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      ID                       |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    QDCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ANCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    NSCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ARCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

*/
struct dnsHeader {  // 96 bit = 12 byte
  uint16_t id;      // 16 bit
  uint16_t qr;      // 01 bit start of 2th byte
  uint16_t opcode;  // 04 bit
  uint16_t aa;      // 01 bit
  uint16_t tc;      // 01 bit
  uint16_t rd;      // 01 bit
  uint16_t ra;      // 01 bit start of 3th byte
  uint16_t z;       // 03 bit
  uint16_t rcode;   // 04 bit
  uint16_t qdcount; // 16 bit
  uint16_t ancount; // 16 bit
  uint16_t nscount; // 16 bit
  uint16_t arcount; // 16 bit

  void (*printDnsHeader)(const struct dnsHeader *self);
};

/* DNS header */
struct dnsHeader *extractDnsHeader(const u_char *dnsHeaderPtr);
uint16_t extractDnsHeaderID(const unsigned char *dnsHeaderPtr);
uint16_t extractDnsHeaderQR(const unsigned char *dnsHeaderPtr);
uint16_t extractDnsHeaderOpCode(const unsigned char *dnsHeaderPtr);
uint16_t extractDnsHeaderAA(const unsigned char *dnsHeaderPtr);
uint16_t extractDnsHeaderTC(const unsigned char *dnsHeaderPtr);
uint16_t extractDnsHeaderRD(const unsigned char *dnsHeaderPtr);
uint16_t extractDnsHeaderRA(const unsigned char *dnsHeaderPtr);
uint16_t extractDnsHeaderZ(const unsigned char *dnsHeaderPtr);
uint16_t extractDnsHeaderRCode(const unsigned char *dnsHeaderPtr);
uint16_t extractDnsHeaderQDCount(const unsigned char *dnsHeaderPtr);
uint16_t extractDnsHeaderANCount(const unsigned char *dnsHeaderPtr);
uint16_t extractDnsHeaderNSCount(const unsigned char *dnsHeaderPtr);
uint16_t extractDnsHeaderARCount(const unsigned char *dnsHeaderPtr);

/* DNS Question

       0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                                               |
    /                     QNAME                     /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QTYPE                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QCLASS                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

*/
struct dnsQuestion {
  u_char *qname;     // variable-length
  uint16_t qnamelen; // length of query name
  uint16_t qtype;    // 16 bit
  uint16_t qclass;   // 16 bit

  // methods
  void (*printDnsQuestion)(const struct dnsQuestion *self);
};

struct dnsQuestion *extractDnsQuestion(const u_char *dnsQuestionPtr);
u_char *extractQueryName(const u_char *questionPtr);
uint16_t extractQueryType(const u_char *questionPtr, int dnsQueryNameLen);
uint16_t extractQueryClass(const u_char *questionPtr, int dnsQueryNameLen);

#endif // DNS_H_
