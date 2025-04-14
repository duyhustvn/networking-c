#include "source-af-packet.h"

#include <linux/if_ether.h>
#include <netinet/in.h>
#include<sys/socket.h>

#define FRAME_SIZE 2048
#define NUM_FRAMES 64

// TODO: update the logic to compute ring params
static int AFPComputeRingParams(AFPTheadVars *ptv, int order) {
    ptv->req.v2.tp_block_size = FRAME_SIZE * NUM_FRAMES; /* Minimal size of contiguous block */
    ptv->req.v2.tp_block_nr = 1; /* Number of blocks */
    ptv->req.v2.tp_frame_size = FRAME_SIZE; /* Size of frame */
    ptv->req.v2.tp_frame_nr = NUM_FRAMES;  /* Total number of frames */

    return 0;    
}

static int AFPCreateSocket(AFPTheadVars *ptv, char *devname, int verbose) {
    // create a raw socket
    ptv->socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (ptv->socket < 0) {
        perror("failed to create raw socket");
        exit(EXIT_FAILURE);
    }
   
    // Setup TPACKET_V2 ring buffer 
    // TODO: Update the order
    AFPComputeRingParams(ptv, 1);

    return 0;
}
