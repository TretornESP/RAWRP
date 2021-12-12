
#include "ip.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

void swap_bytes(uint8_t *a, uint8_t *b) {
    uint8_t tmp = *a;
    *a = *b;
    *b = tmp;
}
#define N_BYTE(x, y) (x >> (8 * y) & 0xff)

void setup_ip(struct ip_conf * conf, uint8_t protocol, uint32_t saddr, uint32_t * options, size_t optlen, size_t mtu) {
    conf->protocol = protocol;
    conf->saddr = htonl(saddr);
    conf->options = options;
    conf->optlen = optlen;
    conf->mtu = mtu;
    printf("Source: %d.%d.%d.%d\n", N_BYTE(conf->saddr, 0), N_BYTE(conf->saddr, 1), N_BYTE(conf->saddr, 2), N_BYTE(conf->saddr, 3));
}

void init_ip(struct ip* ip, struct ip_conf* conf, void* data, size_t len, char* daddr) {
    if (conf->optlen > 10) {
        printf("Too many options\n");
        return;
    }

    inet_pton(AF_INET, daddr, &(ip->daddr));

    ip->version = IP_VERSION;
    ip->ihl = 5+conf->optlen;
    ip->tos = TOS;
    ip->tot_len = ip->ihl*4 + len;
    ip->id = INIT_ID;
    ip->flags = 0;
    ip->frag_off = 0; //Fragmentation aint supported
    ip->ttl = TTL;
    ip->protocol = conf->protocol;
    ip->check = 0;
    ip->saddr = conf->saddr;
    
    if (conf->optlen > 0) { //Revisar
        ip->options = malloc(conf->optlen);
        memset(ip->options, 0, conf->optlen);
    } else {
        ip->options = NULL;
    }

    ip->data = malloc(len);
    memcpy(ip->data, data, len);

    if (ip->tot_len > conf->mtu) {
        printf("Packet too big\n");
        return;
    }
}

void destroy_ip(struct ip* ip) {
    free(ip->data);
}

void data_ip(struct ip* ip, uint8_t * data) {
    size_t bytes_header = 5*4;

    uint8_t nip[bytes_header];
    memcpy(nip, ip, bytes_header);

    swap_bytes(&nip[2], &nip[3]);
    swap_bytes(&nip[4], &nip[5]);

    memcpy(data, &nip, bytes_header);

    if (ip->options != NULL) {
        memcpy(data + bytes_header, ip->options, (ip->ihl - 5)*4);
    }
    
    memcpy(data+bytes_header+(ip->ihl-5)*4, ip->data, ip->tot_len - ip->ihl*4); 
    
}

size_t size_ip(struct ip* ip) {
    return ip->tot_len;
}

void ethertype_ip(uint8_t * type) {
    type[0] = 0x08;
    type[1] = 0x00;
}