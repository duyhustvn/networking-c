#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include "process_file.h"
#include "craft_tcp.h"

#include <libnet/libnet-functions.h>

void usage(char* name) {
    printf("usage: %s [-a source_ip] [-b source_mac] [-c destination_mac] [-d file_name] [-e device_interface]", name);
}

int main(int argc, char **argv) {
    char* programName = "smuf_ip";
    char errbuf[LIBNET_ERRBUF_SIZE];

    // char* devInterface  = getenv("DEVICE_INTERFACE");
    // if (!devInterface) {
    //     errx(1, "ERROR: failed to load device interface from environment");
    //     return -1;
    // }

    // char *fileName = getenv("FILE");;
    // if (!fileName) {
    //     errx(1, "ERROR: failed to load file from environment");
    //     return -1;
    // }

    // char *srcIP = getenv("SOURCE_IP");
    // if (!srcIP) {
    //     errx(1, "ERROR: failed to load source ip from environment");
    //     return -1;
    // } else {
    //     warnx("srcIP: %s", srcIP);
    // }

    // char* srcMac = getenv("SOURCE_MAC"); // mac address of victom
    // if (!srcMac) {
    //     errx(1, "ERROR: failed to load source mac from environment");
    //     return -1;
    // } else {
    //     warnx("srcMac: %s", srcMac);
    // }

    // char *dstMac =  getenv("DEST_MAC"); // ip address of the router that the computer running this program connected to
    // if (!dstMac) {
    //     errx(1, "ERROR: failed to load destination mac from environment");
    //     return -1;
    // } else {
    //     warnx("dstMac: %s", dstMac);
    // }

    char *srcIP, *srcMac, *dstMac;
    char *fileName;
    char* devInterface;
    int c;
    while((c = getopt(argc, argv, "a:b:c:d:e:")) != -1) {
        switch (c) {
            case 'a':
                srcIP = optarg;
                break;
            case 'b':
                srcMac = optarg;
                break;
            case 'c':
                dstMac = optarg;
                break;
            case 'd':
                fileName = optarg;
            case 'e':
                devInterface = optarg;
            default:
                usage(programName);
        }
    }

    printf("srcIP: %s srcMac: %s dstMac: %s fileName: %s devInterface: %s", srcIP, srcMac, dstMac, fileName, devInterface);

    if (!srcIP || !srcMac || !dstMac || !fileName || ! devInterface) {
        usage(programName);
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
    readAndProcessFileByChunk(l, fileName, srcIP, srcMac, dstMac);


    libnet_destroy(l);
    return 0;
}
