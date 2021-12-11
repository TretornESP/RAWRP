#include "dispatchers.h"
#include "ether.h"
#include "ethertypes.h"
#include "arp.h"

#include <stdio.h>
#include <stdlib.h>

//#define LOG


#ifdef LOG
    #define log(...) (printf(__VA_ARGS__))
#else
    #define log(...)
#endif

void master_dispatcher(struct nic* nic, void* buffer, ssize_t size) {
    log("\n========== NEW PACKED ARRIVED ==========\n");

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
                    log("ARP packet received, size=%d\n", data_size);
                    break;
                case ETHERT_IPV4:
                    log("IPv4 frame detected!\n");
                    return;
                    break;      
            }
            free(eth_raw_data);
            break;
        }
        case ETH_INVALID:
            //Invalid ethernet version
            log("Invalid ethernet version\n");
            return;
        default:
            //Unknown ethernet version
            log("Unknown ethernet version\n");
            return;
    } 
    void * eth_data = malloc(data_size);
    get_eth_data(&eth, eth_data, data_size);

    log("\b========== ETH Frame ==========\n");
    log("Source addr: %02x:%02x:%02x:%02x:%02x:%02x\n", eth.sa[0], eth.sa[1], eth.sa[2], eth.sa[3], eth.sa[4], eth.sa[5]);
    log("Dest addr: %02x:%02x:%02x:%02x:%02x:%02x\n", eth.da[0], eth.da[1], eth.da[2], eth.da[3], eth.da[4], eth.da[5]);
    log("Length: %d\n", data_size);
    for (int i = 0; i < data_size; i++) {
        log("%02x ", ((uint8_t*)eth_data)[i]);
    }
    log("\n========== End ETH Frame ==========\n");

    log("\n========== ARP Packet ==========\n");
    struct arp arp;
    parse_arp(&arp, eth_data, data_size);
    log("Hardware type: %d\n", arp.htype);
    log("Protocol type: %d\n", arp.ptype);
    log("Hardware size: %d\n", arp.hlen);
    log("Protocol size: %d\n", arp.plen);
    log("Operation: %d\n", arp.oper);
    log("Sender MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", arp.sha[0], arp.sha[1], arp.sha[2], arp.sha[3], arp.sha[4], arp.sha[5]);
    log("Sender IP: %d.%d.%d.%d\n", arp.spa[0], arp.spa[1], arp.spa[2], arp.spa[3]);
    log("Target MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", arp.tha[0], arp.tha[1], arp.tha[2], arp.tha[3], arp.tha[4], arp.tha[5]);
    log("Target IP: %d.%d.%d.%d\n", arp.tpa[0], arp.tpa[1], arp.tpa[2], arp.tpa[3]);

    log("\n========== End ARP Packet ==========\n");
    struct arp_cache * cache = get_global_arp_cache();
    uint8_t mac[6];
    if (get_arp_cache(&cache, arp.spa, mac, 6, 4) < 0) {
        log("No cache entry found\n");
        add_arp_cache(&cache, arp.spa, arp.sha, 6, 4);
    } else {
        remove_arp_cache(&cache, arp.spa, 4);
        add_arp_cache(&cache, arp.spa, arp.sha, 6, 4);
        log("Cache entry updated\n");
        log("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    log("\n========== PACKET PROCESSED ==========\n");

    pthread_cond_broadcast(&nic->cond);
}

void general_eof_handler(struct nic* nic, void* buffer, ssize_t size) {
    log("EOF\n");
}

void general_error_handler(struct nic* nic, void* buffer, ssize_t size) {
    log("Error\n");
}