#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include "process_file.h"
#include "craft_tcp.h"

#include <libnet/libnet-functions.h>


int main() {
    // char fileName[] = "statics/ip.txt";

    // char *srcIP = getenv("SOURCE_IP");
    // if (!srcIP) {
    //     errx(1, "ERROR: failed to load source ip from environment");
    //     return -1;
    // } else {
    //     warnx("srcIP: %s", srcIP);
    // }

    // char* srcMac = getenv("SOURCE_MAC");
    // if (!srcMac) {
    //     errx(1, "ERROR: failed to load source mac from environment");
    //     return -1;
    // } else {
    //     warnx("srcMac: %s", srcMac);
    // }
    // char srcMac[] = "38:df:eb:6a:9c:10";


    char errbuf[LIBNET_ERRBUF_SIZE];
    char* outputInterface = "wlp0s20f3";
    libnet_t *l = libnet_init(LIBNET_RAW4, outputInterface, errbuf);
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
