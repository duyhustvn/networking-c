#ifndef SOURCE_AF_PACKET_H_
#define SOURCE_AF_PACKET_H_

#include "common.h"

#include <linux/if_packet.h>

#define FRAME_SIZE 2048
#define NUM_FRAMES 64
#define POLL_TIMEOUT -1

enum {
    AFP_READ_OK,
    AFP_READ_FAILURE,
};

enum {
    AFP_FATAL_ERROR = 1,
    AFP_RECOVERABLE_ERROR,
};

typedef struct AFPThreadVars_ {
    int socket; // socket file descriptor

    union AFPpacketReq {
        struct tpacket_req v2; 
#ifdef HAVE_TPACKET_V3
        struct tpacket_req3 v3;
#endif
    } req;

} AFPTheadVars;

int AFPCreateSocket(AFPTheadVars *ptv, char *devname, int verbose);
static int AFPComputeRingParams(AFPTheadVars *ptv, int order);
int AFPGetIfnumByDev(int fd, const char *ifname, int verbose);
void ProcessPacket(unsigned char* , int);
void print_ip_header(unsigned char* , int);
void print_tcp_packet(unsigned char * , int );
void print_udp_packet(unsigned char * , int );
void print_icmp_packet(unsigned char* , int );
void PrintData (unsigned char* , int);
int AFPPacketProcessUsingRingBuffer();
int AFPPacketProcessPoll();

#endif // SOURCE_AF_PACKET_H_
