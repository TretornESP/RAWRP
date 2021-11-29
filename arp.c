#include "arp.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

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

void init_arp(struct arp* arp, uint8_t* mac, uint8_t* ip) {
    uint8_t hlen = 6;
    uint8_t plen = 4;
    
    arp->htype = htons(1);
    arp->ptype = htons(0x0800);
    arp->hlen = hlen;
    arp->plen = plen;
    arp->oper = htons(1);
    arp->sha = (uint8_t*)malloc(hlen);
    arp->spa = (uint8_t*)malloc(plen);
    arp->tha = (uint8_t*)malloc(hlen);
    arp->tpa = (uint8_t*)malloc(plen);

    memcpy(arp->sha, mac, hlen);
    memcpy(arp->spa, ip, plen);
    memcpy(arp->tha, (uint8_t*)"\xff\xff\xff\xff\xff\xff", hlen);
}

void destroy_arp(struct arp* arp) {
    free(arp->sha);
    free(arp->spa);
    free(arp->tha);
    free(arp->tpa);
}
