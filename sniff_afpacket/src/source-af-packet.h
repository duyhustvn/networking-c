#ifndef SOURCE_AF_PACKET_H_
#define SOURCE_AF_PACKET_H_

#include "common.h"
#include "tm-threads-common.h"

#include <linux/if_packet.h>
#include <stdint.h>

#define FRAME_SIZE 2048
#define NUM_FRAMES 64
#define POLL_TIMEOUT 100

#define AFP_IFACE_NAME_LENGTH 48

enum {
    AFP_READ_OK,
    AFP_READ_FAILURE,
};

enum {
    AFP_FATAL_ERROR = 1,
    AFP_RECOVERABLE_ERROR,
};

union thdr {
    struct tpacket2_hdr *h2;
#ifdef HAVE_TPACKET_V3
    struct tpacket3_hdr *h3;
#endif
    void * raw;
};

typedef struct AFPThreadVars_ {
    int socket; // socket file descriptor

    union AFPpacketReq {
        struct tpacket_req v2; 
#ifdef HAVE_TPACKET_V3
        struct tpacket_req3 v3;
#endif
    } req;

    char iface[AFP_IFACE_NAME_LENGTH];

    /* mmap ring buffer */
    unsigned int ring_buflen;
    uint8_t *ring_buf;

    /* data linktype in host order */
    int datalink;

} AFPTheadVars;

int AFPCreateSocket(AFPTheadVars *ptv, char *devname, int verbose);
int ProcessPacket(unsigned char* , int);
void print_ip_header(unsigned char* , int);
void print_tcp_packet(unsigned char * , int );
void print_udp_packet(unsigned char * , int );
void print_icmp_packet(unsigned char* , int );
void PrintData (unsigned char* , int);
int AFPPacketProcessUsingRingBufferNoPolling(void);
int AFPPacketProcessUsingRingBufferPolling(void);
int AFPPacketProcessPoll(void);

#endif // SOURCE_AF_PACKET_H_
