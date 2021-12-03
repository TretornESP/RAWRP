#ifndef _IP_H
#define _IP_H

#include <stdint.h>

#define TTL 69
#define INIT_ID 69420
#define TOS 0
#define IP_VERSION 4
#define ICMP_PROTO 1

struct ip {
    uint8_t version : 4;
    uint8_t ihl : 4;
    uint8_t tos;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t check;
    uint32_t saddr;
    uint32_t daddr;
    uint32_t *options;
    uint32_t *data;
};

struct ip_conf{
    uint8_t protocol;
    uint32_t saddr;
    uint32_t * options;
    size_t optlen;
    size_t mtu;
};

/*
struct fragment {
    uint16_t id;
    uint16_t offset;
    struct ip *father;
    void * data;
}
*/

void setup_ip(struct ip_conf * conf, uint8_t protocol, uint32_t saddr, uint32_t * options, size_t optlen, size_t mtu);
void init_ip(struct ip* ip, struct ip_conf* conf, void* data, size_t len, uint32_t daddr);
void data_ip(struct ip* ip, uint8_t * data);
#endif