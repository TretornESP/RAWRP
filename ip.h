#ifndef _IP_H
#define _IP_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define TTL 69
#define INIT_ID 42069
#define TOS 0
#define IP_VERSION 4
#define ICMP_PROTO 1

struct ip {
    uint8_t ihl : 4;
    uint8_t version : 4;
    uint8_t tos;
    uint16_t tot_len;
    uint16_t id;
    uint8_t flags : 3;
    uint16_t frag_off : 13;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t check;
    uint32_t saddr;
    uint32_t daddr;
    uint32_t *options;
    uint32_t *data;
} __attribute__((packed));

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
void init_ip(struct ip* ip, struct ip_conf* conf, void* data, size_t len, char* daddr);
void data_ip(struct ip* ip, uint8_t * data);
size_t size_ip(struct ip* ip);
void destroy_ip(struct ip* ip);
void ethertype_ip(uint8_t * type);
#endif