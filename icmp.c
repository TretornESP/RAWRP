#include "icmp.h"
#include "randgen.h"
#include <stdio.h>
#include <stdlib.h>

uint16_t calculate_icmp_checksum(uint8_t type, uint8_t code, uint16_t *data, size_t len){
    uint32_t checksum = type << 8 | code;

    for(int i = 0; i < len; i++){
        checksum += data[i];
    }

    return ~(((checksum & 0xf0000)>>16)+ (checksum & 0xffff));
}

void init_icmp(struct icmp *icmp, uint8_t type, uint8_t code) {
    size_t dsize;

    switch (type)
    {
        case ICMP_ECHO_REQUEST: {
            dsize = ICMP_REQUEST_DATA_SIZE;
            break;
        }
        case ICMP_ECHO_REPLY: {
            printf("not supported\n");
            return;
            break;
        }
    }

    icmp->type = type;
    icmp->code = code;
    icmp->id = ICMP_ID;
    icmp->seq = rand16();
    icmp->data = malloc(dsize);
    for (int i = 0; i < dsize; i++) {
        icmp->data[i] = 0x61+ (i%0x17);
    }
    icmp->checksum = calculate_icmp_checksum(type, code, (uint16_t*)icmp->data, dsize);
}

size_t icmp_size(struct icmp *icmp) {
    size_t dsize;
    switch (icmp->type)
    {
        case ICMP_ECHO_REQUEST: {
            dsize = ICMP_REQUEST_DATA_SIZE;
            break;
        }
        case ICMP_ECHO_REPLY: {
            return -1;
            break;
        }
    }
    return 8 + dsize;
}

void destroy_icmp(struct icmp *icmp) {
    free(icmp->data);
}

void data_icmp(struct icmp *icmp, uint8_t* data) {
    memcpy(data, &(icmp->type), 1);
    memcpy(data + 1, &(icmp->code), 1);
    memcpy(data + 2, &(icmp->checksum), 2);
    memcpy(data + 4, &(icmp->id), 2);
    memcpy(data + 6, &(icmp->seq), 2);

    switch (icmp->type) {
        case ICMP_ECHO_REQUEST:
            memcpy(data + 8, icmp->data, ICMP_REQUEST_DATA_SIZE);
            break;
        case ICMP_ECHO_REPLY:
            printf("not supported\n");
            break;
    }
}