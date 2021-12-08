#include "iface.h"

#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

void init_nic(struct nic *nic, char* iface)
{
	struct ifreq ifr;
	
	nic->fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (nic->fd < 0)
    {
        perror("socket");
    }
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
	ioctl(nic->fd, SIOCGIFHWADDR, &ifr);
    memcpy(nic->mac, ifr.ifr_hwaddr.sa_data, 6);
    ioctl(nic->fd, SIOCGIFADDR, &ifr);
    memcpy(nic->ip, ifr.ifr_addr.sa_data + 2, 4);
    ioctl(nic->fd, SIOCGIFINDEX, &ifr);
    nic->index = ifr.ifr_ifindex;
	memcpy(nic->name, iface, strlen(iface));

    struct sockaddr_ll sll;
    pthread_t *thread = malloc(sizeof(pthread_t));
    void *buffer = malloc(NIC_BUFFER);

    memset((void*)&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_protocol = htons(ETH_P_ALL);
    sll.sll_ifindex = nic->index;

    int ret = bind(nic->fd, (struct sockaddr *)&sll, sizeof(sll));
    if (ret < 0) {
        perror("bind");
    }
    
    nic->buffer = buffer;
    nic->buffer_size = NIC_BUFFER;
    nic->thread = thread;

    return;
}

void init_network_listener(struct network_listener *listener, struct nic *nic) {
    listener->nic = nic;
}

void register_network_listener(struct network_listener *nl, callback_t callback, callback_t eof_callback, callback_t error_callback) {
    nl->callback = callback;
    nl->eof_callback = eof_callback;
    nl->error_callback = error_callback;

    pthread_create(nl->nic->thread, NULL, receive_data, (void*)nl);
}
void deregister_network_listener(struct network_listener *listener) {
    pthread_cancel(*(listener->nic->thread));
    pthread_join(*(listener->nic->thread), NULL);
}
void destroy_nic(struct nic *nic) {
    free(nic->buffer);
    free(nic->thread);
    close(nic->fd);
}
void *receive_data(void * args) {
    struct network_listener* nl = (struct network_listener*)args;
    struct nic* nic = nl->nic;
    ssize_t read_bytes;
    while (1) {
        while ((read_bytes = read(nic->fd, nic->buffer, nic->buffer_size)) != 0) {
            if (read_bytes < 0) {
                perror("recv");
                nl->error_callback(nic->buffer, errno);
            } else if (read_bytes == 0) {
                printf("recv: EOF\n");
                nl->eof_callback(nic->buffer, read_bytes);
            }
            printf("recv: %d bytes\n", read_bytes);
            nl->callback(nic->buffer, read_bytes);
        }
    }

    return 0;
}
ssize_t receive_packet(struct nic* nic, void * buffer, size_t size) {
    if (nic->fd < 0) {
        return -1;
    }

    ssize_t read_bytes;

    while (1) {
        read_bytes = read(nic->fd, buffer, size);
        if (read_bytes < 0) {
            perror("recv");
            return -1;
        } else if (read_bytes == 0) {
            printf("recv: EOF\n");
            return read_bytes;
        }
        printf("recv: %d bytes\n", read_bytes);
    }
}
void send_packet(struct nic* nic, void * data, size_t size) {
    if (nic->fd < 0) {
        perror("send: socket");
        return;
    }

    size_t send_len = write(nic->fd, data, size);
    if(send_len<0) {
        printf("Error sending, len=%ld, error=%d\n",send_len,errno);
        perror("sendto");
    } else {
        printf("Raw dump of packet: ");
        //for (int i = 0; i < send_len; i++) {
        //    printf("%.2x ", ((uint8_t*)data)[i]);
        //}

        printf("\nSent %ld/%ld bytes through: %d\n",send_len, size, nic->index);
    }
}
