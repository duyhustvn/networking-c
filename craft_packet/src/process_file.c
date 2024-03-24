
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


void processChunk(char* chunk, libnet_t* l, uint32_t srcIP, uint8_t* srcMac) {
    i++;
    // printf("chunk: %s\n\n", chunk);
    printf("chunk: %d\n", i);
    uint16_t srcPort = 49996; // random or fixed port
    uint16_t dstPort = 443;
    uint32_t seq = 2508113620;
    uint32_t ack = 3567497537;
    uint8_t control = 0x02; // sync

    char *dstIPStr = "20.205.243.166";
    uint32_t dstIP = inet_addr(dstIPStr);

    char *dstMacStr = "00:1d:aa:9b:44:78";
    int r;
    uint8_t* dstMac = libnet_hex_aton(dstMacStr, &r);
    if (dstMac == NULL) {
        // sprintf(errstr, "ERROR: invalid dest MAC address");
        errx(1, "ERROR: invalid dest MAC address");
    }

    char errstr[1024];

    libnet_clear_packet(l);
    // for (int i = 0; i < 10; i++) {
    craftTcpPacket(l, srcPort, dstPort, seq, ack,  control,  srcIP,  dstIP,  srcMac,  dstMac, errstr);
    // }

}

int readAndProcessFileByChunk(libnet_t* l) {
    FILE *f;
    char buffer[BUFFER_SIZE];
    size_t bytesRead;

    char fileName[] = "./statics/ip.txt";

    f = fopen(fileName, "r");
    if (f == NULL) {
        errx(1, "failed to open file %s", fileName);
        return -1;
    }

    char srcIP[] = "192.168.1.6";
    char srcMac[] = "38:df:eb:6a:9c:10";

    uint32_t srcIpInt = inet_addr(srcIP);

    int r;
    uint8_t* srcMacInt = libnet_hex_aton(srcMac, &r);
    if (srcMacInt == NULL) {
        errx(1, "ERROR: invalid source MAC address");
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
           processChunk(chunk, l, srcIpInt, srcMacInt);

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
