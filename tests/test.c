#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint16_t calculate_icmp_checksum(uint8_t type, uint8_t code, uint16_t *data, uint16_t len){
    uint32_t checksum = type << 8 | code;

    for(int i = 0; i < len; i++){
        checksum += data[i];
    }

    return ~(((checksum & 0xf0000)>>16)+ (checksum & 0xffff));
}

int main() {
    uint8_t type = 8;
    uint8_t code = 0;
    uint16_t data[] = {\
        0x0001, 0x0073, 0x6162, 0x6364, 0x6566, 0x6768, 0x696a, 0x6b6c, \
        0x6d6e, 0x6f70, 0x7172, 0x7374, 0x7576, 0x7761, 0x6263, 0x6465, \
        0x6667, 0x6869 \
    };


    uint16_t len = sizeof(data) / sizeof(uint16_t);
    uint16_t checksum = calculate_icmp_checksum(type, code, data, len);
    printf("-> 0x%x\n", checksum);
}