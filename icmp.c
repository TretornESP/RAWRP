#include "icmp.h"
#include "randgen.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

uint16_t
checksum (uint16_t *addr, int len)
{
  int count = len;
  register uint32_t sum = 0;
  uint16_t answer = 0;

  // Sum up 2-byte values until none or only one byte left.
  while (count > 1) {
    sum += *(addr++);
    printf("Sum: %02x%02x%02x%02x\n", sum >> 24, (sum >> 16) & 0xFF, (sum >> 8) & 0xFF, sum & 0xFF);
    count -= 2;
  }

  // Add left-over byte, if any.
  if (count > 0) {
    sum += *(uint8_t *) addr;
  }

  // Fold 32-bit sum into 16 bits; we lose information by doing this,
  // increasing the chances of a collision.
  // sum = (lower 16 bits) + (upper 16 bits shifted right 16 bits)
  while (sum >> 16) {
    sum = (sum & 0xffff) + (sum >> 16);
  }

  printf("SUM: %02x%02x\n", sum >> 8, sum & 0xff);
  printf("~SUM: %02x%02x\n", ~sum >> 8, ~sum & 0xff);

  // Checksum is one's compliment of sum.
  answer = ~sum;

  return (answer);
}


uint16_t calculate_icmp_checksum(uint8_t type, uint8_t code, uint8_t id, uint8_t seq, uint16_t *data, size_t len){
    uint16_t check = (type << 8 | code);
    printf("C: %02x%02x\n", check >> 8, check & 0xFF);
    check += 0;
    printf("C: %02x%02x\n", check >> 8, check & 0xFF);
    check += id;
    printf("C: %02x%02x\n", check >> 8, check & 0xFF);
    check += seq;
    printf("C: %02x%02x\n", check >> 8, check & 0xFF);

    for(int i = 0; i < len; i++){
        check += data[i];
        printf("D[i]: %02x%02x\n", data[i] >> 8, data[i] & 0xFF);
        printf("C[i]: %02x%02x\n", i, check >> 8, check & 0xFF);
    }
    printf("~C: %02x%02x\n", ~(check) >> 8, ~(check) & 0xFF);
    return ~(check);
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

    icmp->checksum = 0;

    uint8_t data[dsize+8];

    data[0] = icmp->type;
    data[1] = icmp->code;
    data[2] = 0;
    data[3] = 0;
    data[4] = icmp->id >> 8;
    data[5] = icmp->id & 0xFF;
    data[6] = icmp->seq >> 8;
    data[7] = icmp->seq & 0xFF;

    for (int i = 0; i < dsize; i++) {
        data[i+8] = icmp->data[i];
    }

    icmp->checksum = checksum((uint16_t*)data, dsize + 8);
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