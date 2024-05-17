
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "craft_tcp.h"
#include "data_queue.h"
#include "process_file.h"
#include "thread_process.h"

// #define BUFFER_SIZE 1024
#define BUFFER_SIZE 1024


void processChunk(IPQueue *q, char* chunk, uint32_t srcIP, uint8_t* srcMac, uint8_t* dstMac) {
    char delim[] = "\n";
    char *ptr = strtok(chunk, delim);
    Data *data;
    while (ptr != NULL) {
        char *dstIPStr = strdup(ptr);

        data = (Data*)malloc(sizeof(Data));
        if (data == NULL) {
            printf("Failed to assign memory\n");
            free(dstIPStr);
            continue;
        }
        data->ip = dstIPStr;
        IPEnqueue(q, data);
        ptr = strtok(NULL, delim);
    }
}

// int readAndProcessFileByChunk(libnet_t* l, char *fileName, char *srcIP, char *srcMac, char *dstMac) {
int readAndProcessFileByChunk(config cfg) {
    FILE *f;
    char buffer[BUFFER_SIZE];
    size_t bytesRead;

    char* filePath = cfg.filePath;
    char* srcIP = cfg.srcIP;
    char* srcMac = cfg.srcMac;
    char *dstMac = cfg.dstMac;

    IPQueue *q = IPQueueAlloc();

    f = fopen(filePath, "r");
    if (f == NULL) {
        errx(1, "failed to open file %s", filePath);
        return -1;
    }

    uint32_t srcIpInt = inet_addr(srcIP);

    int r;
    uint8_t* srcMacInt = libnet_hex_aton(srcMac, &r);
    if (srcMacInt == NULL) {
        errx(1, "ERROR: invalid source MAC address");
        return -1;
    }

    uint8_t* dstMacInt = libnet_hex_aton(dstMac, &r);
    if (dstMacInt == NULL) {
        errx(1, "ERROR: invalid dest MAC address");
        return -1;
    }

    // read file in chunk and process each chunk
    // read 1024 bytes first
    // if it is in the middle of line, continue to read the whole line
    while (1) {
       bytesRead = fread(buffer, 1, BUFFER_SIZE, f);

       if (bytesRead > 0)  {
           char c;
           int i = 0;
           char additional[100];
           while ((c = fgetc(f)) && c != '\n' && c != EOF) {
               additional[i] = c;
               i++;
           }
           additional[i]= '\0';

           char chunk[bytesRead+i];
           strcpy(chunk, buffer);
           strcat(chunk, additional);
           chunk[bytesRead+i] = '\0';

           // process chunk
           processChunk(q, chunk, srcIpInt, srcMacInt, dstMacInt);

           memset(chunk, 0, bytesRead);
       } else {
           // reach end of file
           break;
       }

       memset(buffer, 0, BUFFER_SIZE);
    }

    int numsThreads = 4;
    pthread_t threads[numsThreads];
    struct threadData_ threadDatas[numsThreads];
    for (long t = 0; t < numsThreads; t++) {
        threadDatas[t].threadID = t + 1;
        threadDatas[t].countPackets = 0;
        threadDatas[t].q = q;

        threadDatas[t].srcIp = srcIpInt;
        threadDatas[t].srcMac = srcMacInt;
        threadDatas[t].dstMac = dstMacInt;
    }

    int rc;
    for (long t = 0; t < numsThreads; t++) {
        rc = pthread_create(&threads[t], NULL, process, (void*)&threadDatas[t]);
        if (rc) {
            printf("ERROR in creating thread, return code is %d\n", rc);
            exit(1);
        }
    }

    void *status;
    for (long t = 0; t < numsThreads; t++) {
        rc = pthread_join(threads[t], &status);
        if (rc) {
            printf("ERROR in joining thread return code is %d\n", rc);
            exit(1);
        }

        printf("Main: completed join with thread %ld having status of: %s\n", t, (char *)status);
    }

    int sumPkt = 0;
    for (long t = 0; t < numsThreads; t++) {
        printf("Thread %ld packet: %d\n", t, threadDatas[t].countPackets);
        sumPkt += threadDatas[t].countPackets;
    }
    printf("The total number of package from all thread %d\n", sumPkt);

    // cleanup
    free(srcMacInt);
    free(dstMacInt);

    IPQueueTraversal(q);

    IPQueueFree(q);
    fclose(f);

    return 0;
}
