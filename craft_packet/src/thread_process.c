#include "thread_process.h"
#include <libnet/libnet-functions.h>


void *process(void *threadArg) {
    uint16_t srcPort = 49996; // random or fixed port
    uint16_t dstPort = 443;
    uint32_t seq = 2508113620;
    uint32_t ack = 3567497537;
    uint8_t control = 0x02; // sync


    threadData *data = (threadData *) threadArg;

    int threadID = data->threadID;

    printf("Thread %d starting \n", threadID);

    IPQueue *q = data->q;
    while (!IPQueueEmpty(q)) {
        Data* packet = IPDequeue(q);
        char* ip = packet->ips;

        char *dstIPStr = strdup(ip);
        uint32_t dstIP = inet_addr(dstIPStr);

        char errstr[1024];

        char errbuf[LIBNET_ERRBUF_SIZE];
        char *devInterface = "wlp0s20f3";
        libnet_t *l = libnet_init(LIBNET_RAW4, devInterface, errbuf);
        if (l == NULL) {
            // fprintf(stderr, "ERROR: getLibnetSocket(): libnet init failed %s\n", errbuf);
            printf("ERROR: libnet init failed %s\n", errbuf);
            continue;
        }
        libnet_clear_packet(l);
        craftTcpPacket(l, srcPort, dstPort, seq, ack,  control,  data->srcIp,  dstIP,  data->srcMac,  data->dstMac, errstr);
        libnet_destroy(l);

        // printf("threadID: %d ip: %s\n", threadID, ip);
        data->countPackets++;
    }

    pthread_exit(NULL);
}
