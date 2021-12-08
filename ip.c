
#include "ip.h"

void setup_ip(struct ip_conf * conf, uint8_t protocol, uint32_t saddr, uint32_t * options, size_t optlen, size_t mtu) {
    conf->protocol = protocol;
    conf->saddr = saddr;
    conf->options = options;
    conf->optlen = optlen;
    conf->mtu = mtu;
}

void init_ip(struct ip* ip, struct ip_conf* conf, void* data, size_t len, uint32_t daddr) {
    if (conf->optlen > 10) {
        printf("Too many options\n");
        return;
    }

    ip->version = IP_VERSION;
    ip->ihl = 5+conf->optlen;
    ip->tos = TOS;
    ip->tot_len = ip->ihl*4 + len;
    ip->id = htons(INIT_ID);
    ip->frag_off = 0; //Fragmentation aint supported
    ip->ttl = TTL;
    ip->protocol = conf->protocol;
    ip->check = 0;
    ip->saddr = conf->saddr;
    ip->daddr = daddr;
    ip->options = conf->options;
    ip->data = data;

    if (ip->tot_len > conf->mtu) {
        printf("Packet too big\n");
        return;
    }
}

void data_ip(struct ip* ip, uint8_t * data) {
    memcpy(data, ip, 5);
    memcpy(data+5, ip->options, ip->ihl-5);
    memcpy(data+ip->ihl, ip->data, ip->tot_len - ip->ihl*4);
}