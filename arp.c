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

void init_arp(struct arp* arp, uint8_t* mac, uint8_t* sip, const char* tips) {

    uint8_t tip[4];
    inet_pton(AF_INET, tips, tip);

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
    memcpy(arp->spa, sip, plen);
    memcpy(arp->tha, (uint8_t*)"\x00\x00\x00\x00\x00\x00", hlen);
    memcpy(arp->tpa, tip, plen);
}

void destroy_arp(struct arp* arp) {
    free(arp->sha);
    free(arp->spa);
    free(arp->tha);
    free(arp->tpa);
}

void size_arp(struct arp* arp, uint8_t* size) {
    uint16_t len = 8+2*(arp->hlen+arp->plen);
    size[0] = len;
    size[1] = len >> 8;
}

void ethertype_arp(uint8_t *type) {
    type[0] = 0x08;
    type[1] = 0x06;
}

void data_arp(struct arp* arp, uint8_t* data) {
    memcpy(data, &arp->htype, 2);
    memcpy(data+2, &arp->ptype, 2);
    memcpy(data+4, &arp->hlen, 1);
    memcpy(data+5, &arp->plen, 1);
    memcpy(data+6, &arp->oper, 2);
    memcpy(data+8, arp->sha, arp->hlen);
    memcpy(data+8+arp->hlen, arp->spa, arp->plen);
    memcpy(data+8+arp->hlen+arp->plen, arp->tha, arp->hlen);
    memcpy(data+8+arp->hlen+arp->plen+arp->hlen, arp->tpa, arp->plen);
}