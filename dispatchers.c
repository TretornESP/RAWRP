#include "dispatchers.h"
#include "ether.h"
#include "ethertypes.h"
#include "arp.h"

#include <stdio.h>
#include <stdlib.h>

void master_dispatcher(void* buffer, ssize_t size) {
    printf("\n========== NEW PACKED ARRIVED ==========\n");

    struct eth eth;
    parse_eth(&eth, buffer);
    int data_size = eth.length[0] * 256 + eth.length[1];
    switch (get_eth_version(&eth)) {
        case ETH_802_3:
            break;
        case ETH_II: {
            void * eth_raw_data = malloc(ETH_MAX_FRAME_SIZE);
            get_eth_data(&eth, eth_raw_data, ETH_MAX_FRAME_SIZE);
            
            switch (data_size) {
                case ETHERT_ARP:
                    data_size = get_arp_payload_size(eth_raw_data);
                    printf("ARP packet received, size=%d\n", data_size);
                    break;
                case ETHERT_IPV4:
                    printf("IPv4 frame detected!\n");
                    return;
                    break;      
            }
            free(eth_raw_data);
            break;
        }
        case ETH_INVALID:
            //Invalid ethernet version
            printf("Invalid ethernet version\n");
            return;
        default:
            //Unknown ethernet version
            printf("Unknown ethernet version\n");
            return;
    } 
    void * eth_data = malloc(data_size);
    get_eth_data(&eth, eth_data, data_size);

    printf("\b========== ETH Frame ==========\n");
    printf("Source addr: %02x:%02x:%02x:%02x:%02x:%02x\n", eth.sa[0], eth.sa[1], eth.sa[2], eth.sa[3], eth.sa[4], eth.sa[5]);
    printf("Dest addr: %02x:%02x:%02x:%02x:%02x:%02x\n", eth.da[0], eth.da[1], eth.da[2], eth.da[3], eth.da[4], eth.da[5]);
    printf("Length: %d\n", data_size);
    for (int i = 0; i < data_size; i++) {
        printf("%02x ", ((uint8_t*)eth_data)[i]);
    }
    printf("\n========== End ETH Frame ==========\n");

    printf("\n========== ARP Packet ==========\n");
    struct arp arp;
    parse_arp(&arp, eth_data, data_size);
    printf("Hardware type: %d\n", arp.htype);
    printf("Protocol type: %d\n", arp.ptype);
    printf("Hardware size: %d\n", arp.hlen);
    printf("Protocol size: %d\n", arp.plen);
    printf("Operation: %d\n", arp.oper);
    printf("Sender MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", arp.sha[0], arp.sha[1], arp.sha[2], arp.sha[3], arp.sha[4], arp.sha[5]);
    printf("Sender IP: %d.%d.%d.%d\n", arp.spa[0], arp.spa[1], arp.spa[2], arp.spa[3]);
    printf("Target MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", arp.tha[0], arp.tha[1], arp.tha[2], arp.tha[3], arp.tha[4], arp.tha[5]);
    printf("Target IP: %d.%d.%d.%d\n", arp.tpa[0], arp.tpa[1], arp.tpa[2], arp.tpa[3]);

    printf("\n========== End ARP Packet ==========\n");
    printf("Updating arp cache:\n");
    struct arp_cache * cache = get_global_arp_cache();
    uint8_t mac[6];
    if (overwrite_arp_cache(&cache, arp.spa, mac, 6, 4) < 0) {
        printf("No cache entry found\n");
        add_arp_cache(&cache, arp.spa, arp.sha, 6, 4);
    } else {
        printf("Cache entry updated\n");
        printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    printf("\n========== PACKET PROCESSED ==========\n");
}

void general_eof_handler(void* buffer, ssize_t size) {
    printf("EOF\n");
}

void general_error_handler(void* buffer, ssize_t size) {
    printf("Error\n");
}