#include "crc.h"
#include "ether.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h> //You can delete this if you start implementing fragmentation

void init_eth(struct eth *eth, uint8_t* sa, uint8_t *da, uint8_t *data, uint8_t *type, uint8_t *length) {
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

    #ifdef INCLUDE_PREAMBLE

    for (int i = 0; i < 7; i++) {
        eth->preamble[i] = 0x55;
    }
    eth->sfd = 0x7e;
    #endif


    memcpy(eth->length, type, 2);
    memcpy(eth->sa, sa, 6);
    memcpy(eth->da, da, 6);

    uint8_t* packet = malloc(llength+26);

    #ifdef INCLUDE_PREAMBLE

    memcpy(packet, &eth->preamble, 7);
    memcpy(packet+7, &eth->sfd, 1);

    #endif
    memcpy(packet, &eth->da, 6);
    memcpy(packet+6, &eth->sa, 6);
    memcpy(packet+12, &eth->length, 2);
    memcpy(packet+14, eth->data, llength);
    free(eth->data);
    eth->crc = crc32_byte(packet, llength+14);
    memcpy(packet+llength+14, &eth->crc, 4);

    printf("SENT CRC: %x\n", eth->crc);
    eth->data = packet; 
    //Esto es una mierda, deberia seguir la interfaz de data_arp que te saca el paquete del header
    //Pero como entonces habria que esperar para calcular el crc me da profunda pereza y lo hago asi
 
}

uint8_t get_eth_version(struct eth *eth) {
    uint16_t ethsize = eth->length[0] + eth->length[1]*256;
    if (ethsize < 46) {
        return ETH_INVALID;
    }
    if (ethsize > 1500 && ethsize < 1536) {
        return ETH_INVALID;
    }
    if (ethsize > 1536) {
        return ETH_II;
    }
    return ETH_802_3;
}

void parse_eth(struct eth *eth, uint8_t* data) {
    uint16_t payload_length = data[ETH_PAYLOAD_SIZE_OFFSET] + data[ETH_PAYLOAD_SIZE_OFFSET+1]*256;

    #ifdef INCLUDE_PREAMBLE
    memcpy(eth->preamble, data, 7);
    memcpy(eth->sfd, data+7, 1);
    memcpy(eth->da, data+8, 6);
    memcpy(eth->sa, data+14, 6);
    memcpy(eth->length, data+20, 2);
    memcpy(eth->data, data+22, eth->length[0] + eth->length[1]*256);
    memcpy(eth->crc, data+22+eth->length[0] + eth->length[1]*256, 4);
    #else
    
    eth->data = malloc(payload_length);
    memcpy(eth->da, data, 6);
    memcpy(eth->sa, data+6, 6);
    memcpy(eth->length, data+12, 2);
    memcpy(eth->data, data+14, payload_length);
    eth->crc = (uint32_t)*(data+14+payload_length);
    #endif
}

//Checks if ethernet crc is valid
int check_crc(struct eth *eth) {
    if (eth->crc == 0) {
        return 1;
    }
    uint8_t *packet = malloc(eth->length[0] + eth->length[1]*256 + 26);
    memcpy(packet, &eth->da, 6);
    memcpy(packet+6, &eth->sa, 6);
    memcpy(packet+12, &eth->length, 2);
    memcpy(packet+14, eth->data, eth->length[0] + eth->length[1]*256);
    uint32_t crc = crc32_byte(packet, eth->length[0] + eth->length[1]*256);
    free(packet);
    printf("CRC: %x\n", crc);
    printf("STORED: %x\n", eth->crc);
    return crc == eth->crc;
}

void get_eth_data(struct eth *eth, uint8_t *data, uint16_t size) {
    if (check_crc(eth)) {
        printf("Length: %d\n", size);
        memcpy(data, eth->data, size);
    } else {
        printf("CRC error\n");
    }
}

void destroy_eth(struct eth *eth) {
    free(eth->data);
}
