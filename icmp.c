#include "icmp.h"

uint16_t calculate_icmp_checksum(uint8_t type, uint8_t code, uint16_t *data, size_t len){
    uint32_t checksum = type << 8 | code;

    for(int i = 0; i < len; i++){
        checksum += data[i];
    }

    return ~(((checksum & 0xf0000)>>16)+ (checksum & 0xffff));
}

void init_icmp(struct icmp *icmp, uint8_t type, uint8_t code, uint8_t* data, size_t len) {
    icmp->type = type;
    icmp->code = code;
    icmp->checksum = calculate_icmp_checksum(type, code, (uint16_t)data, len);
    icmp->data = data;
}

void data_icmp(struct icmp *icmp, uint8_t* data) {
    memcpy(data, icmp->type, 1);
    memcpy(data + 1, icmp->code, 1);
    memcpy(data + 2, icmp->checksum, 2);
    memcpy(data + 4, icmp->data, 4);

    switch (icmp->type) {
        case ICMP_ECHO_REQUEST:
            memcpy(data + 8, icmp->data+4, 4);
            break;
        case ICMP_ECHO_REPLY:
            printf("not supported\n");
            break;
    }
}