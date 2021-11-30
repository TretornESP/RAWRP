#include "net.h"
#include "arp.h"
#include "ether.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main() {
    uint8_t length[2];
    uint8_t type[2];
    uint8_t sha[6];
    uint8_t ip[4];
    int interface_index;
 
    get_if_info("enp0s8", sha, ip, &interface_index);

    struct arp arp;
    init_arp(&arp, sha, ip, "192.168.1.101");
    size_arp(&arp, length);
    ethertype_arp(type);
    
    uint8_t * arp_data = malloc((uint16_t)*length); //No se si este cast falla para numeros mas grandes que 2^8 idk
    data_arp(&arp, arp_data);

    struct eth eth;
    init_eth(&eth, sha, (uint8_t*)"\xff\xff\xff\xff\xff\xff", arp_data, type, length);

    send_packet(interface_index, eth.data, *length);

    return 0;
}
