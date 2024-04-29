#include "thread_process.h"


void *process(void *threadArg) {
    threadData *data = (threadData *) threadArg;

    int threadID = data->threadID;

    printf("Thread %d starting \n", threadID);

    IPQueue *q = data->q;
    while (!IPQueueEmpty(q)) {
        Data* packet = IPDequeue(q);
        char* ip = packet->ips;
        printf("ip: %s\n", ip);
        data->countPackets++;
    }

    pthread_exit(NULL);
}
