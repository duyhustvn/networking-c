#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include "process_file.h"
#include "craft_tcp.h"

#include <libnet/libnet-functions.h>


int main() {
    char errbuf[LIBNET_ERRBUF_SIZE];

    char* devInterface  = getenv("DEVICE_INTERFACE");
    if (!devInterface) {
        errx(1, "ERROR: failed to load device interface from environment");
        return -1;
    }

    libnet_t *l = libnet_init(LIBNET_RAW4, devInterface, errbuf);
    if (l == NULL) {
        // fprintf(stderr, "ERROR: getLibnetSocket(): libnet init failed %s\n", errbuf);
        errx(1, "ERROR: getLibnetSocket(): libnet init failed %s\n", errbuf);
        return -1;
    }

    int seedResult = libnet_seed_prand(l);
    if (seedResult == -1) {
        errx(1, "ERROR: seedPrand(): libnet seed prand failed \n");
        return -1;
    }

    printf("l->injection_type: %d\n", l->injection_type);
    readAndProcessFileByChunk(l);


    libnet_destroy(l);
    return 0;
}
