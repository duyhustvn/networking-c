#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include "process_file.h"
#include "craft_tcp.h"
#include "config.h"


#include <libnet/libnet-functions.h>

#include "./util/util_signal.h"
#include "./logger/log.h"

volatile sig_atomic_t sigterm_count = 0;
volatile sig_atomic_t sigsegv_count = 0;
volatile sig_atomic_t sigint_count = 0;

static void signalHandlerSigint(int sig) {
    sigint_count = 1;
}

static void signalHandlerSigsegv(int sig) {
    sigsegv_count++;
    printf("sigsegv_count: %d\n", sigsegv_count);
}

void usage(char* name) {
    printf("usage: %s [-a source_ip] [-b source_mac] [-c destination_mac] [-d file_name] [-e device_interface] [-f timeout]\n\n", name);
}

void initSignalHandler() {
    utilSignalHandlerSetup(SIGINT, signalHandlerSigint);
    utilSignalHandlerSetup(SIGSEGV, signalHandlerSigsegv);
}

int main(int argc, char **argv) {
    initLogFile("./app.log");
    setLogLevel(LOG_NONE);
    logMessage(LOG_INFO, "Start program");

    long seconds, useconds;
    double total_time;
    struct timeval start, end;
    gettimeofday(&start, NULL);

    char* programName = "smuf_ip";
    char errbuf[LIBNET_ERRBUF_SIZE];

    // initSignalHandler();

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
    char* timeoutStr; // the time program run
    int timeout;

    int c;
    while((c = getopt(argc, argv, "a:b:c:d:e:f:")) != -1) {
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
                break;
            case 'e':
                devInterface = optarg;
                break;
            case 'f':
                timeoutStr = optarg;
                timeout = atoi(timeoutStr);
                if (timeout <= 0) {
                    timeout = 5;
                }
                break;
            default:
                usage(programName);
        }
    }

    printf("From argument srcIP: %s srcMac: %s dstMac: %s fileName: %s devInterface: %s timeout: %d \n\n", srcIP, srcMac, dstMac, fileName, devInterface, timeout);

    if (!srcIP || !srcMac || !dstMac || !fileName || ! devInterface) {
        usage(programName);
    }


    config cfg = {.srcIP = srcIP, .srcMac = srcMac, .dstMac = dstMac, .filePath = fileName, .deviceInterface = devInterface, .timeout = timeout};

    libnet_t *l = libnet_init(LIBNET_RAW4, devInterface, errbuf);
    if (l == NULL) {
        errx(1, "ERROR: getLibnetSocket(): libnet init failed %s\n", errbuf);
        return -1;
    }

    int seedResult = libnet_seed_prand(l);
    if (seedResult == -1) {
        errx(1, "ERROR: seedPrand(): libnet seed prand failed \n");
        return -1;
    }

    printf("l->injection_type: %d\n", l->injection_type);
    readAndProcessFileByChunk(cfg);

    libnet_destroy(l);

    printf("sigsegv_count: %d\n", sigsegv_count);

    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    total_time = seconds + useconds / 1e6;
    warnx("FINISH PROGRAM after %f seconds\n", total_time);
    // logMessage(LOG_INFO, "FINISH PROGRAM after seconds\n");

    return 0;
}
