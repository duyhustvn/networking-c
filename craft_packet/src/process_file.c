
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <err.h>
#include <unistd.h>

#include "craft_tcp.h"
#include "data_circle_linked_list.h"
#include "data_queue.h"
#include "process_file.h"
#include "thread_process.h"
#include "util_cpu.h"

// #define BUFFER_SIZE 1024
#define BUFFER_SIZE 1024

uint64_t line = 0;

void processChunk(IPCircleLinkedList *lls[], int numThreads, char* chunk, uint32_t srcIP, uint8_t* srcMac, uint8_t* dstMac) {
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
        data->next = NULL;
        int threadIdx = line % numThreads;
        warnx("threadIdx: %d", threadIdx);
        IPCicleLinkedListInsertAtTheEnd(lls[threadIdx], data);
        ptr = strtok(NULL, delim);
    }

    line++;
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

    int numsThreads;
    uint16_t numCpu = getNumberOfProcessorsOnline();
    warnx("Number of online cpus: %d\n", numCpu);
    if (numCpu == 0) {
        numsThreads = 4;
    } else {
        numsThreads = numCpu;
    }


    IPCircleLinkedList *lls[numsThreads];
    for (int i = 0; i < numsThreads; i++) {
        lls[i] = IPCircleLinkedListAlloc();
    }


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
    long seconds, useconds;
    double total_time;

    warnx("START READING DATA FROM FILE\n");
    struct timeval start, end;
    gettimeofday(&start, NULL);
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
           processChunk(lls, numsThreads, chunk, srcIpInt, srcMacInt, dstMacInt);

           memset(chunk, 0, bytesRead);
       } else {
           // reach end of file
           break;
       }

       memset(buffer, 0, BUFFER_SIZE);
    }

    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    total_time = seconds + useconds / 1e6;
    warnx("FINISH READING DATA FROM FILE after %f seconds\n", total_time);


    pthread_t threads[numsThreads];
    struct threadData_ threadDatas[numsThreads];
    for (long t = 0; t < numsThreads; t++) {
        threadDatas[t].threadID = t + 1;
        threadDatas[t].countPackets = 0;
        threadDatas[t].ll = lls[t];

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


    // https://sites.ualberta.ca/dept/chemeng/AIX-43/share/man/info/C/a_doc_lib/aixprggd/genprogc/term_threads.htm
    sleep(5);

    for (long t = 0; t < numsThreads; t++) {
        pthread_cancel(threads[t]);
    }

    /* Wait for all threads to finish */
    void *status;
    for (long t = 0; t < numsThreads; t++) {
        rc = pthread_join(threads[t], &status);
        if (rc) {
            printf("ERROR in joining thread return code is %d\n", rc);
            exit(1);
        }

        // if (status == PTHREAD_CANCELED) {
        //     warnx("main(): joined to thread %ld, status=PTHREAD_CANCELED", t);
        // } else {
        //     warnx("main(): joined to thread %ld", t);
        // }

        printf("Main: completed join with thread %ld having status of: %s\n", t, (char *)status);
    }

    int sumPkt = 0;
    for (long t = 0; t < numsThreads; t++) {
        printf("Thread %ld packet: %d\n", t, threadDatas[t].countPackets);
        sumPkt += threadDatas[t].countPackets;
    }
    printf("The total number of package from all thread %d\n", sumPkt);


    // for (int i = 0; i < numsThreads; i++) {
    //     printf("\n\nTHREAD %d\n", i);
    //     IPCicleLinkedListTraversal(lls[i]);
    // }

    /*
     * CLEAN UP
     */

    free(srcMacInt);
    free(dstMacInt);


    for (int i = 0; i < numsThreads; i++) {
        printf("CLEAN LINKEDLIST THREAD %d\n", i);
        IPCircleLinkedListFree(lls[i]);
    }

    printf("\n\n");
    // close file
    fclose(f);

    return 0;
}
