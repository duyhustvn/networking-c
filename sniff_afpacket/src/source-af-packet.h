#ifndef SOURCE_AF_PACKET_H_
#define SOURCE_AF_PACKET_H_

#include "common.h"

#include <linux/if_packet.h>

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

static int AFPCreateSocket(AFPTheadVars *ptv, char *devname, int verbose);
static int AFPComputeRingParams(AFPTheadVars *ptv, int order);

#endif // SOURCE_AF_PACKET_H_
