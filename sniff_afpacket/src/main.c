// Reference: https://www.binarytides.com/packet-sniffer-code-in-c-using-linux-sockets-bsd-part-2/
// https://csulrong.github.io/blogs/2022/03/10/linux-afpacket/

#include "source-af-packet.h"

#include <arpa/inet.h>
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
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#define POLL_TIMEOUT -1

#define ETH_P_ALL 0x0003
#define FRAME_SIZE 2048
#define NUM_FRAMES 64

extern FILE *logfile;
volatile bool running = true;

void signal_handler(int signum) {
    if (signum == SIGTERM || signum == SIGINT) {
        printf("Received terminal signal. Stop the program\n");
        running = false;
    }
}

int main(int argc, char *argv[])
{
	printf("program is running at process id: %ld\n", (long)getpid());
    if (argc != 2) {
        printf("Usage: %s [ring|poll]\n", argv[0]);
    }

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    if (strcmp(argv[1], "ring") == 0) {
        AFPPacketProcessUsingRingBuffer();
    } else if (strcmp(argv[1], "poll") == 0) {
        AFPPacketProcessPoll();
    }

	printf("Finished");
	return 0;
}
