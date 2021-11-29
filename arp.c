#include <stdio.h>	//printf
#include <string.h>	//strncpy
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>	//ifreq
#include <unistd.h>	//close
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

struct arp {
    uint16_t htype;
    uint16_t ptype;
    uint8_t hlen;
    uint8_t plen;
    uint16_t oper;
    uint8_t *sha;
    uint8_t *spa;
    uint8_t *tha;
    uint8_t *tpa;
} __attribute__((packed));

void set_arp_sha(struct arp* arp, uint8_t* sha) {
    memcpy(arp->sha, sha, arp->hlen);
}

void set_arp_spa(struct arp* arp, uint8_t* spa) {
    memcpy(arp->spa, spa, arp->plen);
}

void set_arp_tha(struct arp* arp, uint8_t* tha) {
    memcpy(arp->tha, tha, arp->hlen);
}

void set_arp_tpa(struct arp* arp, uint8_t* tpa) {
    memcpy(arp->tpa, tpa, arp->plen);
}

void get_mac_addr(char* iface, unsigned char* mac, unsigned char* ip)
{
	int fd;
	struct ifreq ifr;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);

	ioctl(fd, SIOCGIFHWADDR, &ifr);
    memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);
    ioctl(fd, SIOCGIFADDR, &ifr);
    memcpy(ip, ifr.ifr_addr.sa_data + 2, 4);
	close(fd);
	
	//display mac address
	printf("Mac : %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    printf("IP  : %u.%u.%u.%u\n" , ip[0], ip[1], ip[2], ip[3]);
	return;
}

void init_arp(struct arp* arp, unsigned char* iname) {
    uint8_t hlen = 6;
    uint8_t plen = 4;

    unsigned char mac[hlen];
    unsigned char ip[plen];
    
    arp->htype = htons(1);
    arp->ptype = htons(0x0800);
    arp->hlen = hlen;
    arp->plen = plen;
    arp->oper = htons(1);
    arp->sha = (uint8_t*)malloc(hlen);
    arp->spa = (uint8_t*)malloc(plen);
    arp->tha = (uint8_t*)malloc(hlen);
    arp->tpa = (uint8_t*)malloc(plen);

    get_mac_addr(iname, mac, ip);
    set_arp_sha(arp, mac);
    set_arp_spa(arp, ip);
}

void destroy_arp(struct arp* arp) {
    free(arp->sha);
    free(arp->spa);
    free(arp->tha);
    free(arp->tpa);
}

void request_arp(struct arp* arp, struct sockaddr_in ip) {
    arp->oper = htons(1);
    set_arp_tha(arp, (uint8_t*)"\xff\xff\xff\xff\xff\xff");
    set_arp_tpa(arp, (uint8_t*)&ip.sin_addr.s_addr);
}

void send_arp(unsigned char* ifname, unsigned char* ip) {
    struct arp arp;
    struct sockaddr_in ips;

    inet_pton(AF_INET, ip, &(ips.sin_addr));
    init_arp(&arp, ifname);
    request_arp(&arp, ips);

    unsigned char mac_dst[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    struct sockaddr sa;
    sa.sa_family = AF_PACKET;
    size_t bytes;
    memcpy(sa.sa_data, mac_dst, 6);
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if ((bytes == sendto(sockfd, &arp, sizeof(arp), 0, (struct sockaddr*)&addr, sizeof(addr))) < 0) {
        perror("sendto");
        exit(1);
    }
    bind(sockfd, (struct sockaddr*)&sa, sizeof(sa));
    write(sockfd, &arp, sizeof(arp));
    close(sockfd);
    destroy_arp(&arp);
}

int main() {
    send_arp("eth0", "192.168.20.2");
    return 0;
}