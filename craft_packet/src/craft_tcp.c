// https://linuxtips.ca/index.php/2022/05/09/craft-packets-with-libnet/


#include <stdio.h>
#include <stdint.h>

#include <err.h>

#include "craft_tcp.h"

#include <libnet/libnet-functions.h>

int craftTcpPacket(libnet_t* l, uint16_t srcPort, uint16_t dstPort, uint32_t seq, uint32_t ack, uint8_t control, char* srcIP, char* dstIP, char* srcMac, char* dstMac, char* errstr) {
    libnet_clear_packet(l);

    libnet_ptag_t etherTag; /* Return code handle for ethernet header */
    libnet_ptag_t ipTag; /* Return code handle for IP header */
    libnet_ptag_t tcpTag; /* Return code handle for TCP */

    /*
    ** BUILD TCP
    */
    tcpTag = libnet_build_tcp(srcPort, dstPort, seq,  ack,  control, 0 /*win*/, 0 /*sum*/, 0 /*urg*/, LIBNET_TCP_H, NULL /*payload*/, 0 /*payload_s*/, l, 0 /*ptag*/);
    if (tcpTag == -1) {
        // sprintf(errstr, "ERROR: on TCP packet craft: %s", libnet_geterror(l));
        errx(1, "ERROR: on TCP packet craft: %s", libnet_geterror(l));
        return -1;
    }

    /*
    **  BUILD IPV4
    */
    uint32_t src_ip_int = inet_addr(srcIP);
    uint32_t dst_ip_int = inet_addr(dstIP);
    ipTag = libnet_build_ipv4(
        LIBNET_IPV4_H/*length*/,
        0 /*TOS*/,
        libnet_get_prand(LIBNET_PRu16) /*IP ID*/,
        0 /*IP Frag*/,
        64/*TTL*/,
        IPPROTO_TCP/*protocol*/,
        0 /*checksum*/,
        src_ip_int /*source ip*/,
        dst_ip_int /*dest ip*/,
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
    int r;
    uint8_t* src_mac_int = libnet_hex_aton(srcMac, &r);
    if (src_mac_int == NULL) {
        errx(1, "ERROR: invalid source MAC address");
        return -1;
    }
    uint8_t* dst_mac_int = libnet_hex_aton(dstMac, &r);
    if (dst_mac_int == NULL) {
        // sprintf(errstr, "ERROR: invalid dest MAC address");
        errx(1, "ERROR: invalid dest MAC address");
        return -1;
    }

    etherTag = libnet_build_ethernet(
        dst_mac_int /*dest mac*/,
        src_mac_int /*source mac*/,
        ETHERTYPE_IP, NULL /*pointer to payload*/,
        0 /*size of payload*/,
        l /*libnet handle*/,
        0 /*libnet protocol tag*/);

    // if (etherTag == -1) {
    //     // sprintf(errstr, "ERROR: on Ethernet packet craft: %s", libnet_geterror(l));
    //     errx(1, "ERROR: on Ethernet packet craft: %s", libnet_geterror(l));
    //     return -1;
    // }

    libnet_toggle_checksum(l, ipTag, LIBNET_ON);

    int bytesSent = libnet_write(l);
    if (bytesSent == -1) {
        // sprintf(errstr, "ERROR: write to : %s", libnet_geterror(l));
        errx(1, "ERROR: write to : %s", libnet_geterror(l));
        return -1;
    } else {
        warn("send ok");
    }

    /* shutdown the interface */
    // libnet_destroy(l);
    return 0;
}
