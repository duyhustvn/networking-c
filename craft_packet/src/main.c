#include <stdio.h>
#include <err.h>

#include "process_file.h"
#include "craft_tcp.h"

#include <libnet/libnet-functions.h>

int main() {
    char* fileName = "ip.txt";

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

    readAndProcessFileByChunk(fileName, l);

    libnet_destroy(l);
    return 0;
}
