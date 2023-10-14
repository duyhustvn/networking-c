#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dns.h"
#include "utils.h"

uint16_t extractDnsHeaderID(const unsigned char *dnsHeaderPtr) {
  return conv2BytesToInt(dnsHeaderPtr);
};

uint16_t extractDnsHeaderQR(const unsigned char *dnsHeaderPtr) {
  // byte 2 bit 7
  // 0x80 = 1000.0000
  return dnsHeaderPtr[2] >> 7;
};

uint16_t extractDnsHeaderOpCode(const unsigned char *dnsHeaderPtr) {
  // byte 2 bit 6->3
  // 0x78 = 0111.1000
  return (dnsHeaderPtr[2] & 0x78) >> 3;
};

uint16_t extractDnsHeaderAA(const unsigned char *dnsHeaderPtr) {
  // byte 2 bit 2
  // 0x04 0000.0100
  return (dnsHeaderPtr[2] & 0x04) >> 2;
};

uint16_t extractDnsHeaderTC(const unsigned char *dnsHeaderPtr) {
  // byte 2 bit 1
  // 0x02 0000.0010
  return (dnsHeaderPtr[2] & 0x02) >> 1;
};

uint16_t extractDnsHeaderRD(const unsigned char *dnsHeaderPtr) {
  // byte 2 bit 0
  // 0x01 0000.0001
  return (dnsHeaderPtr[2] & 0x01);
};

uint16_t extractDnsHeaderRA(const unsigned char *dnsHeaderPtr) {
  // byte 3 bit 7
  // 0x80 = 1000.0000
  return dnsHeaderPtr[3] >> 7;
};

uint16_t extractDnsHeaderZ(const unsigned char *dnsHeaderPtr) {
  // byte 3 bit 6 -> 4
  // 0x70 = 0111.0000
  return (dnsHeaderPtr[3] & 0x70) >> 4;
};

uint16_t extractDnsHeaderRCode(const unsigned char *dnsHeaderPtr) {
  // byte 3 bit 3 -> 0
  // 0x0F = 0000.1111
  return (dnsHeaderPtr[3] & 0x0F);
};

uint16_t extractDnsHeaderQDCount(const unsigned char *dnsHeaderPtr) {
  return conv2BytesToInt(dnsHeaderPtr + 4);
};

uint16_t extractDnsHeaderANCount(const unsigned char *dnsHeaderPtr) {
  return conv2BytesToInt(dnsHeaderPtr + 6);
};

uint16_t extractDnsHeaderNSCount(const unsigned char *dnsHeaderPtr) {
  return conv2BytesToInt(dnsHeaderPtr + 8);
};

uint16_t extractDnsHeaderARCount(const unsigned char *dnsHeaderPtr) {
  return conv2BytesToInt(dnsHeaderPtr + 10);
};

void printDnsHeader(const struct dnsHeader *self) {

  printf("%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s\n",
         "id", "qr", "opcode", "aa", "tc", "rd", "ra", "z", "rcode", "qdcount",
         "ancount", "nscount", "arcount");

  printf("%-10x%-10d%-10d%-10d%-10d%-10d%-10d%-10d%-10d%-10d%-10d%-10d%-10d\n",
         self->id, self->qr, self->opcode, self->aa, self->tc, self->rd,
         self->ra, self->z, self->rcode, self->qdcount, self->ancount,
         self->nscount, self->arcount);
};

struct dnsHeader *extractDnsHeader(const unsigned char *dnsHeaderPtr) {
  struct dnsHeader *dnsHdr =
      (struct dnsHeader *)malloc(sizeof(struct dnsHeader));

  dnsHdr->id = extractDnsHeaderID(dnsHeaderPtr);
  dnsHdr->qr = extractDnsHeaderQR(dnsHeaderPtr);
  dnsHdr->opcode = extractDnsHeaderOpCode(dnsHeaderPtr);
  dnsHdr->aa = extractDnsHeaderAA(dnsHeaderPtr);
  dnsHdr->tc = extractDnsHeaderTC(dnsHeaderPtr);
  dnsHdr->rd = extractDnsHeaderRD(dnsHeaderPtr);
  dnsHdr->ra = extractDnsHeaderRA(dnsHeaderPtr);
  dnsHdr->z = extractDnsHeaderZ(dnsHeaderPtr);
  dnsHdr->rcode = extractDnsHeaderRCode(dnsHeaderPtr);
  dnsHdr->qdcount = extractDnsHeaderQDCount(dnsHeaderPtr);
  dnsHdr->ancount = extractDnsHeaderANCount(dnsHeaderPtr);
  dnsHdr->nscount = extractDnsHeaderNSCount(dnsHeaderPtr);
  dnsHdr->arcount = extractDnsHeaderARCount(dnsHeaderPtr);

  dnsHdr->printDnsHeader = printDnsHeader;

  return dnsHdr;
};

/*
 query name: a domain name represented as a sequence of labels,
             where each label consists of a length octet followed
             by that number of octets.
 For example:
 www.example.com = 3www7example3com0
                 = 03 77 77 77 07 65 78 61 6d 70 6c 65 03 63 6f 6d 00
 in which: www = 77 77 77
           example = 65 78 61 6d 70 6c 65
           com = 63 6f 6d

 github.com = 6github3com0
            = 06 67 69 74 68 75 62 03 63 6f 6d 00
 in which: github = 67 69 74 68 75 62
           com = 63 6f 6d

 The domain name terminates with the
 zero length octet for the null label of the root.  Note
 that this field may be an odd number of octets; no
 padding is used.
*/
u_char *extractQueryName(const u_char *questionPtr) {
  u_char *queryName = NULL;
  const u_char *curChar = questionPtr;
  int queryNameLen = 0; // length
  int queryNameCap = 0; // capacity

  while (*curChar) {
    unsigned char octetCount = *curChar;
    for (int i = 1; i <= octetCount; i++) {
      // capacity <= len
      if (queryNameCap <= queryNameLen) {
        if (queryNameCap == 0) {
          queryNameCap = 1;
        } else {
          queryNameCap *= 2;
        }
        queryName = realloc(queryName, queryNameCap);
      }

      queryName[queryNameLen++] = curChar[i];
    }

    curChar += octetCount + 1;
    if (*curChar) {
      if (queryNameCap <= queryNameLen) {
        queryNameCap *= 2;
        queryName = realloc(queryName, queryNameCap);
      }

      queryName[queryNameLen++] = '.';
    }
  }

  if (queryNameCap <= queryNameLen) {
    queryNameCap += 1;
    queryName = realloc(queryName, queryNameCap);
  }
  queryName[queryNameLen] = '\0';

  return queryName;
};

void printDnsQuestion(const struct dnsQuestion *self) {
  printf("----- DNS Question -----\n");
  printf("%-50s%-50s%-50s\n", "Query Name", "Query Type", "Query Class");
  printf("%-50s%-50d%-50d\n", self->qname, self->qtype, self->qclass);

  printf("\n\n");
};

struct dnsQuestion *extractDnsQuestion(const u_char *dnsQuestionPtr) {
  struct dnsQuestion *dnsQues =
      (struct dnsQuestion *)malloc(sizeof(struct dnsQuestion));

  dnsQues->qname = extractQueryName(dnsQuestionPtr);
  dnsQues->qnamelen = strlen((const char *)dnsQues->qname) + 2;
  // why + 2 because for example: len(github.com) = 10
  // but it is presented as: 6github3com0 = 12

  dnsQues->qtype = extractQueryType(dnsQuestionPtr, dnsQues->qnamelen);
  dnsQues->qclass = extractQueryClass(dnsQuestionPtr, dnsQues->qnamelen);

  dnsQues->printDnsQuestion = printDnsQuestion;

  return dnsQues;
};

/*
  query type:   a two octet code which specifies the type of the query.
                The values for this field include all codes valid for a
                TYPE field, together with some more general codes which
                can match more than one type of RR.

 */
uint16_t extractQueryType(const u_char *dnsQuestionPtr, int dnsQueryNameLen) {
  // printHex(dnsQuestionPtr + dnsQueryNameLen + 2, 2);
  return conv2BytesToInt(dnsQuestionPtr + dnsQueryNameLen);
};

/*
  query class: a two octet code that specifies the class of the query.
               for example, the QCLASS field is IN for the Internet.
*/
uint16_t extractQueryClass(const u_char *questionPtr, int dnsQueryNameLen) {
  return conv2BytesToInt(questionPtr + dnsQueryNameLen + 2);
};
