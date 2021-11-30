#include "net.h"
#include "arp.h"
#include "ether.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <errno.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

int get_if_info(char* iface, uint8_t* mac, uint8_t* ip, int* index)
{
	int fd;
	struct ifreq ifr;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        perror("socket");
        return -1;
    }
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
	ioctl(fd, SIOCGIFHWADDR, &ifr);
    memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);
    ioctl(fd, SIOCGIFADDR, &ifr);
    memcpy(ip, ifr.ifr_addr.sa_data + 2, 4);
    ioctl(fd, SIOCGIFINDEX, &ifr);
    *index = ifr.ifr_ifindex;
	close(fd);
	
	//display mac address
	printf("Mac : %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    printf("IP  : %u.%u.%u.%u\n" , ip[0], ip[1], ip[2], ip[3]);
	return 0;
}

void send_packet(int index, void * data, size_t size) {
    int sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        perror("socket");
        return;

    }
    struct sockaddr_ll sll;

    memset((void*)&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;   
    sll.sll_ifindex = index;
    sll.sll_protocol = htons(ETH_P_ALL);
    
    bind(sockfd, (struct sockaddr*)&sll, sizeof(sll));
    size_t send_len = write(sockfd, data, size);
    if(send_len<0) {
        printf("Error sending, len=%d, error=%d\n",send_len,errno);
        perror("sendto");
    } else {
        for (int i = 0; i < send_len; i++) {
            printf("%.2x ", ((uint8_t*)data)[i]);
        }
        printf("Sent %d/%d bytes through: %d\n",send_len, size, index);
    }
    close(sockfd);
}

