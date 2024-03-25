// https://linuxtips.ca/index.php/2022/05/09/craft-packets-with-libnet/


#include <stdio.h>
#include <stdint.h>

#include <err.h>

#include "craft_tcp.h"

#include <libnet/libnet-functions.h>


int craftTcpPacket(libnet_t* l, uint16_t srcPort, uint16_t dstPort, uint32_t seq, uint32_t ack, uint8_t control, uint32_t srcIP, uint32_t dstIP, uint8_t* srcMac, uint8_t* dstMac, char* errstr) {
    libnet_ptag_t etherTag; /* Return code handle for ethernet header */
    libnet_ptag_t ipTag; /* Return code handle for IP header */
    libnet_ptag_t tcpTag; /* Return code handle for TCP */

    /*
    ** BUILD TCP
    */
    // printf("build tcp\n");
    tcpTag = libnet_build_tcp(srcPort, dstPort, seq,  ack,  control, 0 /*win*/, 0 /*sum*/, 0 /*urg*/, LIBNET_TCP_H, NULL /*payload*/, 0 /*payload_s*/, l, 0 /*ptag*/);
    if (tcpTag == -1) {
        // sprintf(errstr, "ERROR: on TCP packet craft: %s", libnet_geterror(l));
        errx(1, "ERROR: on TCP packet craft: %s", libnet_geterror(l));
        return -1;
    }

    /*
    **  BUILD IPV4
    */
    // printf("build ip\n");
    ipTag = libnet_build_ipv4(
        LIBNET_IPV4_H/*length*/,
        0 /*TOS*/,
        libnet_get_prand(LIBNET_PRu16) /*IP ID*/,
        0 /*IP Frag*/,
        64/*TTL*/,
        IPPROTO_TCP/*protocol*/,
        0 /*checksum*/,
        srcIP /*source ip*/,
        dstIP /*dest ip*/,
        NULL /*payload*/,
        0 /*payload size*/,
        l /*libnet handle*/,
        0);

    if (ipTag == -1) {
        // sprintf(errstr, "ERROR: on IP packet craft: %s", libnet_geterror(l));
        errx(1, "ERROR: on IP packet craft: %s", libnet_geterror(l));
        return -1;
    }


    /*
    ** BUILD ETHERNET
     */
    // printf("build ethernet\n");
    etherTag = libnet_build_ethernet(
        dstMac /*dest mac*/,
        srcMac /*source mac*/,
        ETHERTYPE_IP, NULL /*pointer to payload*/,
        0 /*size of payload*/,
        l /*libnet handle*/,
        0 /*libnet protocol tag*/);

    // if (etherTag == -1) {
    //     // sprintf(errstr, "ERROR: on Ethernet packet craft: %s", libnet_geterror(l));
    //     errx(1, "ERROR: on Ethernet packet craft: %s", libnet_geterror(l));
    //     return -1;
    // }

    // printf("toggle check sum\n");
    libnet_toggle_checksum(l, ipTag, LIBNET_ON);

    // printf("sending packet\n");
    int bytesSent = libnet_write(l);
    if (bytesSent == -1) {
        // sprintf(errstr, "ERROR: write to : %s", libnet_geterror(l));
        errx(1, "ERROR: write to : %s", libnet_geterror(l));
        return -1;
    }
    // else {
    //     warn("send ok");
    // }

    return 0;
}
