#include "arp.h"
#include "ip.h"
#include "icmp.h"
#include "ether.h"
#include "iface.h"
#include "ethertypes.h"
#include "dispatchers.h"
#include "randgen.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <arpa/inet.h>

#define N_BYTE(x, y) (x >> (8 * y) & 0xff)
//ip_packet.saddr >> 24 & 0xff

void rawrp(struct nic* nic, char* tips) {

    printf("Nic initialized\n");

    //ARP initialization
    struct arp arp;
    uint8_t length[2];
    uint8_t type[2];
    init_arp(&arp, nic->mac, nic->ip, tips);
    size_arp(&arp, length);
    ethertype_arp(type);

    size_t llenght = length[0] << 8 | length[1];
    uint8_t * arp_data = malloc(llenght); //No se si este cast falla para numeros mas grandes que 2^8 idk
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


void rawicmp(struct nic* nic, char * tips) {
    struct icmp icmp;
    uint8_t * icmp_data = malloc(32+4);
    icmp_data[0] = 0x00;
    icmp_data[1] = 0x69;
    icmp_data[2] = 0x04;
    icmp_data[3] = 0x20;

    for (int i = 4; i < 32+4; i++) {
        icmp_data[i] = 0x61+ (i%0x16);
    }
    for (int i = 0; i < 32+4; i++) {
        printf("%02x ", icmp_data[i]);
    }

    init_icmp(&icmp, 0x08, 0x00);

    printf("ICMP initialized\n");
    printf("type: %02x\n", icmp.type);
    printf("code: %02x\n", icmp.code);
    printf("checksum: %02x\n", icmp.checksum);
    printf("ID(BE): %02x %02x\n", (icmp.id & 0xff00) >> 8, icmp.id & 0x00ff);
    printf("ID(LE): %02x %02x\n", (icmp.id & 0x00ff), (icmp.id & 0xff00) >> 8);
    printf("SEQ(BE): %02x %02x\n", (icmp.seq & 0xff00) >> 8, icmp.seq & 0x00ff);
    printf("SEQ(LE): %02x %02x\n", (icmp.seq & 0x00ff), (icmp.seq & 0xff00) >> 8);

    size_t packet_size = icmp_size(&icmp);
    uint8_t * icmp_packet = malloc(packet_size);
    data_icmp(&icmp, icmp_packet);

    printf("Data: ");
    for (int i = 8; i < packet_size; i++) {
        printf("%02x ", icmp_packet[i]);
    }
    printf("\n");

    struct ip_conf ipc;
    uint32_t sip = nic->ip[0] << 24 | nic->ip[1] << 16 | nic->ip[2] << 8 | nic->ip[3];
    printf("SIP %d.%d.%d.%d\n", nic->ip[0], nic->ip[1], nic->ip[2], nic->ip[3]);
    setup_ip(&ipc, 0x1, sip, 0, 0, 1500);

    struct ip ip_packet;
    init_ip(&ip_packet, &ipc, icmp_packet, 64+32, tips);
    printf("=============================\n");
    printf("IP Initialized\n");
    printf("Version: %d\n", ip_packet.version);
    printf("IHL: 0x%02x\n", ip_packet.ihl);
    printf("TOS: 0x%02x\n", ip_packet.tos);
    printf("Length: 0x%02x\n", ip_packet.tot_len);
    printf("ID: %d\n", ip_packet.id);
    printf("Flags: 0x%02x\n", ip_packet.flags);
    printf("Fragment offset: %d\n", ip_packet.frag_off);
    printf("TTL: %d\n", ip_packet.ttl);
    printf("Protocol: %d\n", ip_packet.protocol);
    printf("Checksum: 0x%02x\n", ip_packet.check);
    printf("Source: %d.%d.%d.%d\n", N_BYTE(ip_packet.saddr, 0), N_BYTE(ip_packet.saddr, 1), N_BYTE(ip_packet.saddr, 2), N_BYTE(ip_packet.saddr, 3));
    printf("Destination: %d.%d.%d.%d\n",N_BYTE(ip_packet.daddr, 0), N_BYTE(ip_packet.daddr, 1), N_BYTE(ip_packet.daddr, 2), N_BYTE(ip_packet.daddr, 3));

    uint8_t * ip_packet_data = malloc(ip_packet.tot_len);
    data_ip(&ip_packet, ip_packet_data);

    for (int i = 0; i < ip_packet.tot_len; i++) {
        printf("%02x ", ip_packet_data[i]);
    }
    printf("\n");

    uint8_t target_ip[4];
    uint8_t mac[6];
    inet_pton(AF_INET, tips, target_ip);

    struct arp_cache * arpc = get_global_arp_cache();

    int sent = 0;
    while (get_arp_cache(&arpc, target_ip, mac, 6, 4) < 0) {
        printf("ARP cache not found for target %d.%d.%d.%d\n", target_ip[0], target_ip[1], target_ip[2], target_ip[3]);
        print_arp_cache(arpc);
        if (sent == 0) {
            sent = 1;
            rawrp(nic, tips);
        }
        pthread_mutex_lock(&nic->mutex);
        pthread_cond_wait(&nic->cond, &nic->mutex);
        printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        pthread_mutex_unlock(&nic->mutex);
    }

    struct eth eth;
    uint8_t length[2];
    uint8_t type[2];
    ethertype_ip(type);
    size_ip(&ip_packet, length);

    init_eth(&eth, nic->mac, mac, ip_packet_data, type, length);
    send_packet(nic, eth.data, *length);

    printf("Packet sent\n");

    destroy_eth(&eth);
    free(ip_packet_data);
    destroy_ip(&ip_packet);
    free(icmp_packet);
    destroy_icmp(&icmp);

    printf("Resources freed\n");

}

int main() {

    init_randgen();

    struct nic nic;
    init_nic(&nic, "enp0s8");

    struct arp_cache * cache = get_global_arp_cache();
    init_arp_cache(cache);
    
    struct network_listener listener;
    init_network_listener(&listener, &nic);
    register_network_listener(&listener, master_dispatcher, general_eof_handler, general_error_handler);
    
    rawicmp(&nic, "192.168.1.101");

    while(1);

/*
    while (1) {
        
        print_arp_cache(cache);
        sleep(3);
    }
*/
    return 0;
}
