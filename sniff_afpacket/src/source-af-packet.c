#include "source-af-packet.h"

#include <cstring>
#include <errno.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <netinet/in.h>
#include<sys/socket.h>
#include<net/if.h>
#include<sys/ioctl.h>

#define FRAME_SIZE 2048
#define NUM_FRAMES 64

int AFPGetIfnumByDev(int fd, const char *ifname, int verbose) {
    struct ifreq ifr;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

    if (ioctl(fd, SIOCGIFINDEX, &ifr) == -1) {
        errx(EXIT_FAILURE, "%s: failed to find interface: %s", ifname, strerror(errno));
    }

    return ifr.ifr_ifindex;
}

// TODO: update the logic to compute ring params
static int AFPComputeRingParams(AFPTheadVars *ptv, int order) {
    ptv->req.v2.tp_block_size = FRAME_SIZE * NUM_FRAMES; /* Minimal size of contiguous block */
    ptv->req.v2.tp_block_nr = 1; /* Number of blocks */
    ptv->req.v2.tp_frame_size = FRAME_SIZE; /* Size of frame */
    ptv->req.v2.tp_frame_nr = NUM_FRAMES;  /* Total number of frames */

    return RESULT_OK;    
}

static int AFPSetupRing(AFPTheadVars *ptv, char *devname) {
    int val;

#ifdef HAVE_TPACKET_V3
    val = TPACKET_V3;
#else 
    val = TPACKET_V2;
#endif

    // Setup TPACKET_V2 ring buffer 
    if (AFPComputeRingParams(ptv, 1) != RESULT_OK) {
        errx(EXIT_FAILURE, "%s: ring parameters are incorrect.", devname);
    };

    if (setsockopt(ptv->socket, SOL_PACKET, PACKET_RX_RING, &(ptv->req.v2), sizeof(ptv->req.v2)) < 0) {
        printf("%s: failed to activate TPACKET_V2/TPACKET_V3 on packet socket: %s", devname, strerror(errno));
        return RESULT_FAILURE;
    }

    return RESULT_OK;
}

static int AFPCreateSocket(AFPTheadVars *ptv, char *devname, int verbose) {
    int if_idx; // interface index


    // create a raw socket
    ptv->socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (ptv->socket < 0) {
        perror("failed to create raw socket");
        goto socket_err;
    }
    
    if_idx = AFPGetIfnumByDev(ptv->socket, devname, verbose);
    if (if_idx == -1) {
        goto socket_err; 
    }

    /*bind socket*/
    struct sockaddr_ll bind_address = {
        .sll_family = AF_PACKET,
        .sll_protocol = htons(ETH_P_ALL),
        .sll_ifindex = if_idx,
    };

    if (bind(ptv->socket, (struct sockaddr *)&bind_address, sizeof(bind_address)) < 0) {
        goto socket_err;
    }
    
    int ret = AFPSetupRing(ptv, devname);
    if (ret == RESULT_FAILURE) {
        goto socket_err;
    }
    
    return RESULT_OK;

socket_err:
    close(ptv->socket);
    ptv->socket = -1;
    return RESULT_FAILURE;
}
