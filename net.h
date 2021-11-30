#ifndef _NET_H
#define _NET_H
#include <stdint.h>
#include <stdlib.h>

int get_if_info(char* iface, uint8_t* mac, uint8_t* ip, int* index);
void send_packet(int index, void * data, size_t size);

#endif