#include "arp.h"
#include "ether.h"
#include "iface.h"
#include "ethertypes.h"
#include "dispatchers.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

void rawrp(struct nic* nic, char* tips) {

    printf("Nic initialized\n");

    //ARP initialization
    struct arp arp;
    uint8_t length[2];
    uint8_t type[2];
    uint8_t * arp_data = malloc((uint16_t)*length); //No se si este cast falla para numeros mas grandes que 2^8 idk
    init_arp(&arp, nic->mac, nic->ip, tips);
    size_arp(&arp, length);
    ethertype_arp(type);
    data_arp(&arp, arp_data);

    printf("ARP initialized\n");

    //Ethernet initialization
    struct eth eth;
    init_eth(&eth, nic->mac, (uint8_t*)"\xff\xff\xff\xff\xff\xff", arp_data, type, length);
    send_packet(nic, eth.data, *length);

    printf("Packet sent\n");

    //Freeing resources
    destroy_eth(&eth);
    free(arp_data);
    if (arp.sha == 0) {
        printf("SHA\n");
    } else {
        //print first 6 elements of arp.sha
        printf("SHA: %02x:%02x:%02x:%02x:%02x:%02x\n", arp.sha[0], arp.sha[1], arp.sha[2], arp.sha[3], arp.sha[4], arp.sha[5]);
    }
    destroy_arp(&arp);

    printf("Resources freed\n");
}

int main() {
    struct nic nic;
    init_nic(&nic, "enp0s8");
    struct network_listener listener;
    init_network_listener(&listener, &nic);
    register_network_listener(&listener, master_dispatcher, general_eof_handler, general_error_handler);
    
    rawrp(&nic, "192.168.1.101");

    struct arp_cache * cache = get_global_arp_cache();

    while (1) {
        if (cache == NULL) {
            cache = get_global_arp_cache();
        }
        print_arp_cache(cache);
        sleep(3);
    }
    return 0;
}
