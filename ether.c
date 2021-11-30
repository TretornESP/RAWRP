#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "crc.h"
#include "ether.h"

void eth(struct eth *eth, uint8_t* sa, uint8_t *da, uint8_t *data, uint8_t *type, uint8_t *length) {
    uint16_t llength = *length;
    eth->data = malloc(llength);

    if (llength < 46) {
        llength = 46;
        free(eth->data);
        eth->data = malloc(llength);
        length[0] = llength;
        length[1] = llength >> 8;
    }
    if (llength > 1500) {
        llength = 1500;
        free(eth->data);
        eth->data = malloc(llength);
        printf("WARNING: fragmentation isn't supported yet\n");
        length[0] = llength;
        length[1] = llength >> 8;
    }
    memcpy(eth->data, data, llength);
/*
    for (int i = 0; i < 7; i++) {
        eth->preamble[i] = 0x55;
    }
    eth->sfd = 0x7e;

*/

    memcpy(eth->length, type, 2);
    memcpy(eth->sa, sa, 6);
    memcpy(eth->da, da, 6);


    uint8_t* packet = malloc(llength+26);

/*
    memcpy(packet, &eth->preamble, 7);
    memcpy(packet+7, &eth->sfd, 1);*/
    memcpy(packet, &eth->da, 6);
    memcpy(packet+6, &eth->sa, 6);
    memcpy(packet+12, &eth->length, 2);
    memcpy(packet+14, eth->data, llength);

    eth->crc = crc32_byte(packet, llength+14);
    memcpy(packet+llength+14, &eth->crc, 4);
    eth->data = packet;
    printf("\n");
}