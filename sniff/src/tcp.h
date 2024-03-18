#ifndef TCP_H_
#define TCP_H_

// reference: https://www.noction.com/blog/tcp-flags
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct tcpHeaderFlags {
  bool ns; /* nonce sum experimental flags used to help protect against
            * acceidental or malicious concealment of marked packets from sender
            **/

  bool cwr; /* congestion window reduced is used by the sending host to indicate
             * that it received a packet with ECE flat set
             **/
  bool ece; /* ecn echo indicates that TCP peer is ECN capable. ECN allow
             * routers to inform the TCP endpoints that their transmit buffers
             * are filling due to congestion. This allow TCP endpoints to slow
             * their data transmission to prevent packet loss
             **/
  bool urg; /* urgent says that the data should be treated with priority over
             * other data
             **/
  bool ack; /* acknowledgement is used to confirm that the data packet have been
             * received
             **/
  bool psh; /* push flag tell an appllication that the data should be
             * transmitted immediately, and we do not want to wait to fill the
             * entire TCP segment
             **/
  bool rst; /* reset flag resets connection. When host receives this, it must
             * terminate the connection right away. This is only used when there
             * are unrecoverable erros, and it is not a normal way to finish TCP
             * connection
             **/
  bool sync; /* sync flag used in the initial 3-way handshake where both parties
              * generate the initial sequence number bool fin;
              **/
  bool fin; /* fin flag used to end the TCP connection. TCP is a full-duplex, so
             * both the sender and receiver must use the FIN flag to end the
             * connection. This is the standard method of how both parties end
             * the connection
             **/

  // methods
};

struct tcpHeader {
  unsigned int srcPort;        // 2 byte
  unsigned int dstPort;        // 2 byte
  unsigned int seqNumber;      // 4 byte
  unsigned int ackNumber;      // 4 byte
  unsigned int offset;         // 4 bit
  unsigned int reserved;       // 3 bit
  struct tcpHeaderFlags flags; // 9 bit
  unsigned int windown;  /* 2 byte specifies how many bytes the receiver is
                          * willing to receive.
                          **/
  unsigned int checksum; // 2 byte
  unsigned int urgentPointer;
  /* 2 byte urgentPointer used when URG flag has been set,
   * the urgent
   * pointer is used to indicate where the urgent data ends
   **/

  // methods
  void (*printTcpHeader)(struct tcpHeader *self);
};

struct tcpHeader *extractTcpHeader(unsigned char *tcpHeaderPtr);
unsigned int extractSrcPort(unsigned char *tcpHeaderPtr);
unsigned int extractDstPort(unsigned char *tcpHeaderPtr);
unsigned int extractSequenceNumber(unsigned char *tcpHeaderPtr);
unsigned int extractAckNumber(unsigned char *tcpHeaderPtr);
unsigned int extractOffset(unsigned char *tcpHeaderPtr);
unsigned int extractReserved(unsigned char *tcpHeaderPtr);
// flags
bool extractFlagNs(unsigned char *tcpHeaderPtr);
bool extractFlagCwr(unsigned char *tcpHeaderPtr);
bool extractFlagEce(unsigned char *tcpHeaderPtr);
bool extractFlagUrg(unsigned char *tcpHeaderPtr);
bool extractFlagAck(unsigned char *tcpHeaderPtr);
bool extractFlagPsh(unsigned char *tcpHeaderPtr);
bool extractFlagRst(unsigned char *tcpHeaderPtr);
bool extractFlagSync(unsigned char *tcpHeaderPtr);
bool extractFlagFin(unsigned char *tcpHeaderPtr);

unsigned int extractWindowSize(unsigned char *tcpHeaderPtr);
unsigned int extractChecksum(unsigned char *tcpHeaderPtr);
unsigned int extractUrgentPointer(unsigned char *tcpHeaderPtr);

#endif // TCP_H_
