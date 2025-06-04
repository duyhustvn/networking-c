#include "source-af-packet.h"
#include "stats.h"
#include "decode.h"

#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <net/ethernet.h>	
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>	
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>


FILE *logfile;
extern volatile bool running;

struct sockaddr_in source,dest;
int tcp=0,udp=0,icmp=0,others=0,igmp=0,total=0,i,j;

static int AFPGetIfnumByDev(int fd, const char *ifname, int verbose) {
    struct ifreq ifr;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

    if (ioctl(fd, SIOCGIFINDEX, &ifr) == -1) {
        errx(EXIT_FAILURE, "%s: failed to find interface: %s", ifname, strerror(errno));
    }

    return ifr.ifr_ifindex;
}

static int AFPGetDevLinktype(int fd, const char *ifname) {
    struct ifreq ifr;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

    if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1) {
        errx(EXIT_FAILURE, "%s: failed to find interface: %s", ifname, strerror(errno));
    }

    switch (ifr.ifr_hwaddr.sa_family) {
        case ARPHRD_LOOPBACK:
            return LINKTYPE_ETHERNET;
        case ARPHRD_PPP:
        case ARPHRD_NONE:
            return LINKTYPE_RAW;
        default:
            return ifr.ifr_hwaddr.sa_family;
    }
}

// TODO: update the logic to compute ring params
static int AFPComputeRingParams(AFPThreadVars *ptv, int order) {
    ptv->req.v2.tp_block_size = FRAME_SIZE * NUM_FRAMES; /* Minimal size of contiguous block */
    ptv->req.v2.tp_block_nr = 1; /* Number of blocks */
    ptv->req.v2.tp_frame_size = FRAME_SIZE; /* Size of frame */
    ptv->req.v2.tp_frame_nr = NUM_FRAMES;  /* Total number of frames */

    return RESULT_OK;    
}

static int AFPSetupRing(AFPThreadVars *ptv, char *devname) {
    int val;

#ifdef HAVE_TPACKET_V3
    val = TPACKET_V3;
#else 
    val = TPACKET_V2;
#endif

    // Setup TPACKET_V2 ring buffer 
    if (AFPComputeRingParams(ptv, 1) != RESULT_OK) {
        errx(EXIT_FAILURE, "%s: ring parameters are incorrect.", devname);
    };

    // Create ring buffer with above param
    // the ring buffer will receive the frame from card with zero-copy from kernel to user-space
    if (setsockopt(ptv->socket, SOL_PACKET, PACKET_RX_RING, &(ptv->req.v2), sizeof(ptv->req.v2)) < 0) {
        printf("%s: failed to activate TPACKET_V2/TPACKET_V3 on packet socket: %s", devname, strerror(errno));
        return RESULT_FAILURE;
    }

    return RESULT_OK;
}

int AFPCreateSocket(AFPThreadVars *ptv, char *devname, int verbose) {
    int if_idx; // interface index

    // create a raw socket
    // AF_PACKET: work at layer 2, allow to access directly to Ethernet frame
    // SOCK_RAW: allows access to raw frame content
    // ETH_P_ALL: receive all protocol (IP, ARP ...)
    ptv->socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (ptv->socket < 0) {
        perror("failed to create raw socket");
        goto socket_err;
    }
    
    if_idx = AFPGetIfnumByDev(ptv->socket, devname, verbose);
    if (if_idx == -1) {
        goto socket_err; 
    }

    /* bind socket to interface */
    struct sockaddr_ll bind_address = {
        .sll_family = AF_PACKET,
        .sll_protocol = htons(ETH_P_ALL),
        .sll_ifindex = if_idx,
    };

    if (bind(ptv->socket, (struct sockaddr *)&bind_address, sizeof(bind_address)) < 0) {
        goto socket_err;
    }
    
    int ret = AFPSetupRing(ptv, devname);
    if (ret == RESULT_FAILURE) {
        goto socket_err;
    }

    ptv->datalink = AFPGetDevLinktype(ptv->socket, devname);
    
    return RESULT_OK;

socket_err:
    close(ptv->socket);
    ptv->socket = -1;
    return RESULT_FAILURE;
}


int ProcessPacket(unsigned char* buffer, int size)
{
	//Get the IP Header part of this packet , excluding the ethernet header
	struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
	++total;
	switch (iph->protocol) //Check the Protocol and do accordingly...
	{
		case 1:  //ICMP Protocol
			++icmp;
			// print_icmp_packet( buffer , size);
			break;

		case 2:  //IGMP Protocol
			++igmp;
			break;

		case 6:  //TCP Protocol
			++tcp;
			print_tcp_packet(buffer , size);
			break;

		case 17: //UDP Protocol
			++udp;
			// print_udp_packet(buffer , size);
			break;

		default: //Some Other Protocol like ARP etc.
			++others;
			break;
	}
	// printf("TCP : %d   UDP : %d   ICMP : %d   IGMP : %d   Others : %d   Total : %d\r", tcp , udp , icmp , igmp , others , total);
	return 0;
}

void print_ethernet_header(unsigned char* Buffer, int Size)
{
	struct ethhdr *eth = (struct ethhdr *)Buffer;

	fprintf(logfile , "\n");
	fprintf(logfile , "Ethernet Header\n");
	fprintf(logfile , "   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] );
	fprintf(logfile , "   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
	fprintf(logfile , "   |-Protocol            : %u \n",(unsigned short)eth->h_proto);
}

void print_ip_header(unsigned char* Buffer, int Size)
{
	print_ethernet_header(Buffer , Size);

	unsigned short iphdrlen;

	struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr) );
	iphdrlen =iph->ihl*4;

	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iph->saddr;

	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = iph->daddr;

	fprintf(logfile , "\n");
	fprintf(logfile , "IP Header\n");
	fprintf(logfile , "   |-IP Version        : %d\n",(unsigned int)iph->version);
	fprintf(logfile , "   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
	fprintf(logfile , "   |-Type Of Service   : %d\n",(unsigned int)iph->tos);
	fprintf(logfile , "   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));
	fprintf(logfile , "   |-Identification    : %d\n",ntohs(iph->id));
	//fprintf(logfile , "   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);
	//fprintf(logfile , "   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);
	//fprintf(logfile , "   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment);
	fprintf(logfile , "   |-TTL      : %d\n",(unsigned int)iph->ttl);
	fprintf(logfile , "   |-Protocol : %d\n",(unsigned int)iph->protocol);
	fprintf(logfile , "   |-Checksum : %d\n",ntohs(iph->check));
	fprintf(logfile , "   |-Source IP        : %s\n",inet_ntoa(source.sin_addr));
	fprintf(logfile , "   |-Destination IP   : %s\n",inet_ntoa(dest.sin_addr));
}

void print_tcp_packet(unsigned char* Buffer, int Size)
{
	unsigned short iphdrlen;

	struct iphdr *iph = (struct iphdr *)( Buffer  + sizeof(struct ethhdr) );
	iphdrlen = iph->ihl*4;

	struct tcphdr *tcph=(struct tcphdr*)(Buffer + iphdrlen + sizeof(struct ethhdr));

	int header_size =  sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;

	fprintf(logfile , "\n\n***********************TCP Packet*************************\n");

	print_ip_header(Buffer,Size);

	fprintf(logfile , "\n");
	fprintf(logfile , "TCP Header\n");
	fprintf(logfile , "   |-Source Port      : %u\n",ntohs(tcph->source));
	fprintf(logfile , "   |-Destination Port : %u\n",ntohs(tcph->dest));
	fprintf(logfile , "   |-Sequence Number    : %u\n",ntohl(tcph->seq));
	fprintf(logfile , "   |-Acknowledge Number : %u\n",ntohl(tcph->ack_seq));
	fprintf(logfile , "   |-Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
	//fprintf(logfile , "   |-CWR Flag : %d\n",(unsigned int)tcph->cwr);
	//fprintf(logfile , "   |-ECN Flag : %d\n",(unsigned int)tcph->ece);
	fprintf(logfile , "   |-Urgent Flag          : %d\n",(unsigned int)tcph->urg);
	fprintf(logfile , "   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
	fprintf(logfile , "   |-Push Flag            : %d\n",(unsigned int)tcph->psh);
	fprintf(logfile , "   |-Reset Flag           : %d\n",(unsigned int)tcph->rst);
	fprintf(logfile , "   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn);
	fprintf(logfile , "   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
	fprintf(logfile , "   |-Window         : %d\n",ntohs(tcph->window));
	fprintf(logfile , "   |-Checksum       : %d\n",ntohs(tcph->check));
	fprintf(logfile , "   |-Urgent Pointer : %d\n",tcph->urg_ptr);
	fprintf(logfile , "\n");
	fprintf(logfile , "                        DATA Dump                         ");
	fprintf(logfile , "\n");

	fprintf(logfile , "IP Header\n");
	PrintData(Buffer,iphdrlen);

	fprintf(logfile , "TCP Header\n");
	PrintData(Buffer+iphdrlen,tcph->doff*4);

	fprintf(logfile , "Data Payload\n");
	PrintData(Buffer + header_size , Size - header_size );

	fprintf(logfile , "\n###########################################################");
}

void print_udp_packet(unsigned char *Buffer , int Size)
{

	unsigned short iphdrlen;

	struct iphdr *iph = (struct iphdr *)(Buffer +  sizeof(struct ethhdr));
	iphdrlen = iph->ihl*4;

	struct udphdr *udph = (struct udphdr*)(Buffer + iphdrlen  + sizeof(struct ethhdr));

	int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof udph;

	fprintf(logfile , "\n\n***********************UDP Packet*************************\n");

	print_ip_header(Buffer,Size);

	fprintf(logfile , "\nUDP Header\n");
	fprintf(logfile , "   |-Source Port      : %d\n" , ntohs(udph->source));
	fprintf(logfile , "   |-Destination Port : %d\n" , ntohs(udph->dest));
	fprintf(logfile , "   |-UDP Length       : %d\n" , ntohs(udph->len));
	fprintf(logfile , "   |-UDP Checksum     : %d\n" , ntohs(udph->check));

	fprintf(logfile , "\n");
	fprintf(logfile , "IP Header\n");
	PrintData(Buffer , iphdrlen);

	fprintf(logfile , "UDP Header\n");
	PrintData(Buffer+iphdrlen , sizeof udph);

	fprintf(logfile , "Data Payload\n");

	//Move the pointer ahead and reduce the size of string
	PrintData(Buffer + header_size , Size - header_size);

	fprintf(logfile , "\n###########################################################");
}

void print_icmp_packet(unsigned char* Buffer , int Size)
{
	unsigned short iphdrlen;

	struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr));
	iphdrlen = iph->ihl * 4;

	struct icmphdr *icmph = (struct icmphdr *)(Buffer + iphdrlen  + sizeof(struct ethhdr));

	int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof icmph;

	fprintf(logfile , "\n\n***********************ICMP Packet*************************\n");

	print_ip_header(Buffer , Size);

	fprintf(logfile , "\n");

	fprintf(logfile , "ICMP Header\n");
	fprintf(logfile , "   |-Type : %d",(unsigned int)(icmph->type));

	if((unsigned int)(icmph->type) == 11)
	{
		fprintf(logfile , "  (TTL Expired)\n");
	}
	else if((unsigned int)(icmph->type) == ICMP_ECHOREPLY)
	{
		fprintf(logfile , "  (ICMP Echo Reply)\n");
	}

	fprintf(logfile , "   |-Code : %d\n",(unsigned int)(icmph->code));
	fprintf(logfile , "   |-Checksum : %d\n",ntohs(icmph->checksum));
	//fprintf(logfile , "   |-ID       : %d\n",ntohs(icmph->id));
	//fprintf(logfile , "   |-Sequence : %d\n",ntohs(icmph->sequence));
	fprintf(logfile , "\n");

	fprintf(logfile , "IP Header\n");
	PrintData(Buffer,iphdrlen);

	fprintf(logfile , "UDP Header\n");
	PrintData(Buffer + iphdrlen , sizeof icmph);

	fprintf(logfile , "Data Payload\n");

	//Move the pointer ahead and reduce the size of string
	PrintData(Buffer + header_size , (Size - header_size) );

	fprintf(logfile , "\n###########################################################");
}

void PrintData (unsigned char* data , int Size)
{
	int i , j;
	for(i=0 ; i < Size ; i++)
	{
		if( i!=0 && i%16==0)   //if one line of hex printing is complete...
		{
			fprintf(logfile , "         ");
			for(j=i-16 ; j<i ; j++)
			{
				if(data[j]>=32 && data[j]<=128)
					fprintf(logfile , "%c",(unsigned char)data[j]); //if its a number or alphabet

				else fprintf(logfile , "."); //otherwise print a dot
			}
			fprintf(logfile , "\n");
		}

		if(i%16==0) fprintf(logfile , "   ");
			fprintf(logfile , " %02X",(unsigned int)data[i]);

		if( i==Size-1)  //print the last spaces
		{
			for(j=0;j<15-i%16;j++)
			{
			  fprintf(logfile , "   "); //extra spaces
			}

			fprintf(logfile , "         ");

			for(j=i-i%16 ; j<=i ; j++)
			{
				if(data[j]>=32 && data[j]<=128)
				{
				  fprintf(logfile , "%c",(unsigned char)data[j]);
				}
				else
				{
				  fprintf(logfile , ".");
				}
			}

			fprintf(logfile ,  "\n" );
		}
	}
}

int AFPPacketProcessUsingRingBufferNoPolling(void) {
    int saddr_size , data_size;
	struct sockaddr saddr;
	struct pollfd fds;
	int ret;
	int if_idx; // index of network interface

	logfile=fopen("log_ring_buffer_.txt","w");
	if(logfile==NULL)
	{
		printf("Unable to create log_ring_buffer.txt file.");
	}
	printf("Starting...\n");

	char* dev_interface = getenv("DEVICE_INTERFACE");
	if (dev_interface == NULL) {
		errx(1, "ERROR: failed to load environment device interface");
		exit(EXIT_FAILURE);
	}

    // AF_PACKET: work at layer 2, allow to access directly to Ethernet frame
    // SOCK_RAW: allows access to raw frame content
    // ETH_P_ALL: receive all protocol (IP, ARP ...)
	int sockfd = socket(AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;
	if(sockfd < 0) {
		perror("Socket Error");
		exit(EXIT_FAILURE);
	}

    // TPACKET_V2 ring buffer param
    struct tpacket_req req = {
        .tp_block_size = FRAME_SIZE * NUM_FRAMES,
        .tp_block_nr = 1,
        .tp_frame_size = FRAME_SIZE,
        .tp_frame_nr = NUM_FRAMES,
    };

    // Create ring buffer with above param
    // the ring buffer will receive the frame from card with zero-copy from kernel to user-space
    if (setsockopt(sockfd, SOL_PACKET, PACKET_RX_RING, &req, sizeof(req)) < 0) {
        perror("setsockopt");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Map ring buffer kernel to user space
    void *ring_buffer = mmap(NULL,
                        req.tp_block_size * req.tp_block_nr,
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED,
                        sockfd,
                        0);

    if (ring_buffer == MAP_FAILED) {
        perror("mmap");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Bind socket to interface
    struct sockaddr_ll sll = {
        .sll_family = AF_PACKET,
        .sll_protocol = htons(ETH_P_ALL),
        .sll_ifindex = AFPGetIfnumByDev(sockfd, dev_interface, 0),
    };

    if (bind(sockfd, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
        perror("bind");
        munmap(ring_buffer, req.tp_block_size * req.tp_block_nr);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Capturing on interface %s...\n", dev_interface);

    unsigned int frame_num = 0;
    struct tpacket_hdr *header;

    Stats stats = {0};
    init_stats(&stats);

	while(running)
	{
        // Get exactly 1 frame from ring buffer
        header = (struct tpacket_hdr *)(ring_buffer + (frame_num * FRAME_SIZE));

        // Check if the frame is filled content by kernel and transfer it to user space
        // if not may be the kernel is filling the frame to ring buffer slot
        // or there is no frame in that ring buffer slot
        if (!(header->tp_status & TP_STATUS_USER)) {
            usleep(1000);
            continue;
        }

        // Access packet data (starts after header)
        unsigned char *packet_data = (unsigned char *)header + header->tp_mac;
        ProcessPacket(packet_data, header->tp_len);

        stats.packets_processed++;
        stats.bytes_processed += header->tp_len;

        // Release frame back to kernel
        header->tp_status = TP_STATUS_KERNEL;
        frame_num = (frame_num + 1) % NUM_FRAMES;
        
	}

    print_stats(&stats);
    munmap(ring_buffer, req.tp_block_size * req.tp_block_nr);
	close(sockfd);
    return 0;
}



int AFPPacketProcessUsingRingBufferPolling(void) {
    int saddr_size , data_size;
	struct sockaddr saddr;
	struct pollfd fds;
	int r;
	int if_idx; // index of network interface

	logfile=fopen("log_ring_buffer_poll.txt","w");
	if(logfile==NULL)
	{
		printf("Unable to create log_ring_buffer_poll.txt file.");
	}
	printf("Starting...\n");

	char* dev_interface = getenv("DEVICE_INTERFACE");
	if (dev_interface == NULL) {
		errx(1, "ERROR: failed to load environment device interface");
		exit(EXIT_FAILURE);
	}

    // AFPThreadVars*ptv = malloc(sizeof(AFPThreadVars));
    // if (!ptv) {
    //     errx(1, "Failed to allocate memory for thread vars");
    // }
    // AFPCreateSocket(ptv, dev_interface, 0);

	int sockfd = socket(AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ; // socket file description
	if(sockfd < 0) {
		perror("Socket Error");
		exit(EXIT_FAILURE);
	}

    // Set up TPACKET_V2 ring buffer
    struct tpacket_req req = {
        .tp_block_size = FRAME_SIZE * NUM_FRAMES,
        .tp_block_nr = 1,
        .tp_frame_size = FRAME_SIZE,
        .tp_frame_nr = NUM_FRAMES,
    };

    if (setsockopt(sockfd, SOL_PACKET, PACKET_RX_RING, &req, sizeof(req)) < 0) {
        perror("setsockopt");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Map ring buffer to user space
    void *ring_buffer = mmap(NULL, req.tp_block_size * req.tp_block_nr,
                        PROT_READ | PROT_WRITE, MAP_SHARED, sockfd, 0);
    if (ring_buffer == MAP_FAILED) {
        perror("mmap");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Bind to interface
    struct sockaddr_ll sll = {
        .sll_family = AF_PACKET,
        .sll_protocol = htons(ETH_P_ALL),
        .sll_ifindex = AFPGetIfnumByDev(sockfd, dev_interface, 0),
    };

    if (bind(sockfd, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
        perror("bind");
        munmap(ring_buffer, req.tp_block_size * req.tp_block_nr);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Capturing on interface %s...\n", dev_interface);

    unsigned int frame_num = 0;
    struct tpacket_hdr *header;

    Stats stats = {0};
    init_stats(&stats);

    // setup poll structure
    fds.fd = sockfd;
    fds.events = POLLIN;

	while(running)
	{
        // Wait for packets using poll
        r = poll(&fds, 1, POLL_TIMEOUT);

        if (r > 0 && (fds.events & (POLLHUP|POLLRDHUP|POLLERR|POLLNVAL))) {
            // r > 0: events occured
            // POLLHUP: hang up - peer closed its end of the channel
            // POLLRDHUP: remote peer closed connection
            // POLLERR: error condition on the file descriptor
            // POLLNVAL: invalid file descriptor
            continue;
        } else if (r > 0) {
            header = (struct tpacket_hdr *)(ring_buffer + (frame_num * FRAME_SIZE));

            // Check if packet is ready
            if (!(header->tp_status & TP_STATUS_USER)) {
                // No more packets ready
                break;
            }

            // Access packet data (starts after header)
            unsigned char *packet_data = (unsigned char *)header + header->tp_mac;
            r = ProcessPacket(packet_data, header->tp_len);

            stats.packets_processed++;
            stats.bytes_processed += header->tp_len;

            // Release frame back to kernel
            header->tp_status = TP_STATUS_KERNEL;
            frame_num = (frame_num + 1) % NUM_FRAMES;
        } else if (r == 0) {
            // Timeout occurred, no packets
            continue;
        } else if (r < 0) {
            perror("poll");
            break;
        }

	}

    print_stats(&stats);
    munmap(ring_buffer, req.tp_block_size * req.tp_block_nr);
	close(sockfd);
    return 0;
}


int AFPPacketProcessPoll(void) {
    int saddr_size , data_size;
	struct sockaddr saddr;
	struct pollfd fds;
	int ret;
	int if_idx; // index of network interface

	unsigned char *buffer = (unsigned char *) malloc(65536); //Its Big!

	logfile=fopen("log_poll.txt","w");
	if(logfile==NULL)
	{
		printf("Unable to create log.txt file.");
	}
	printf("Starting...\n");

	char* dev_interface = getenv("DEVICE_INTERFACE");
	if (dev_interface == NULL) {
		errx(1, "ERROR: failed to load environment device interface");
		return 1;
	}

	int sockfd = socket(AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ; // socket file description
	//setsockopt(sockfd , SOL_SOCKET , SO_BINDTODEVICE , "eth0" , strlen("eth0")+ 1 );
	if(sockfd < 0)
	{
		//Print the error with proper message
		perror("Socket Error");
		return 1;
	}

	if_idx = AFPGetIfnumByDev(sockfd, dev_interface, 0);
	if (if_idx == -1) {
		return 1;
	}

	// bind socket to a specific interface
	struct sockaddr_ll bind_address;
	memset(&bind_address, 0, sizeof(bind_address));
	bind_address.sll_family = AF_PACKET;
	bind_address.sll_protocol = htons(ETH_P_ALL);
	bind_address.sll_ifindex = if_idx;

	if (bind(sockfd, (struct sockaddr*)&bind_address, sizeof(bind_address)) == -1) {
		perror("bind socket error");
	}

	// prepare for polling
	fds.fd = sockfd;
	fds.events = POLLIN;

    Stats stats = {0};
    init_stats(&stats);

	while(running)
	{
		ret = poll(&fds, 1, POLL_TIMEOUT);
		if (ret < 0) {
			continue;
		}

		if (fds.revents & POLLIN) {
			// Receive the packet
			saddr_size = sizeof saddr;
			//Receive a packet
			data_size = recvfrom(sockfd , buffer , 65536 , 0 , &saddr , (socklen_t*)&saddr_size);
			if(data_size <0 )
			{
				printf("Recvfrom error , failed to get packets\n");
				return 1;
			}
			//Now process the packet
			ProcessPacket(buffer , data_size);
            stats.packets_processed++;
            stats.bytes_processed += data_size;
		}
	}

    print_stats(&stats);
	close(sockfd);
    return 0;
}
