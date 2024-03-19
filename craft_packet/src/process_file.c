
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "craft_tcp.h"
#include "process_file.h"

// #define BUFFER_SIZE 1024
#define BUFFER_SIZE 1024

int i = 0;

void processChunk(char* chunk, libnet_t* l) {
    i++;
    // printf("chunk: %s\n\n", chunk);
    printf("chunk: %d\n", i);
    uint16_t srcPort = 49996; // random or fixed port
    uint16_t dstPort = 443;
    uint32_t seq = 2508113620;
    uint32_t ack = 3567497537;
    uint8_t control = 0x02; // sync

    char *srcIP = "";
    char *dstIP = "";
    char *srcMac = "";
    char *dstMac = "";


    char errstr[1024];
    craftTcpPacket(l, srcPort, dstPort, seq, ack,  control,  srcIP,  dstIP,  srcMac,  dstMac, errstr);
}

int readAndProcessFileByChunk(char* fileName, libnet_t* l) {
    FILE *f;
    char buffer[BUFFER_SIZE];
    size_t bytesRead;

    f = fopen(fileName, "r");
    if (f == NULL) {
        errx(1, "failed to open file %s", fileName);
        return -1;
    }

    // read file in chunk and process each chunk
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
           processChunk(chunk, l);

           memset(chunk, 0, bytesRead);
       } else {
           // reach end of file
           break;
       }

       memset(buffer, 0, BUFFER_SIZE);
    }

    fclose(f);

    return 0;
}
