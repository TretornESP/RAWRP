#include "net.h"
#include "arp.h"
#include "ip.h"
#include "icmp.h"
#include "ether.h"

#include <stdlib.h>
#include <stdint.h>

void rawrp(char* ifname, char* tips) {
    uint8_t length[2];
    uint8_t type[2];
    uint8_t sha[6];
    uint8_t ip[4];
    int interface_index;

    get_if_info(ifname, sha, ip, &interface_index);

    struct arp arp;
    init_arp(&arp, sha, ip, tips);
    size_arp(&arp, length);
    ethertype_arp(type);

    uint8_t * arp_data = malloc((uint16_t)*length); //No se si este cast falla para numeros mas grandes que 2^8 idk
    data_arp(&arp, arp_data);

    struct eth eth;
    init_eth(&eth, sha, (uint8_t*)"\xff\xff\xff\xff\xff\xff", arp_data, type, length);

    send_packet(interface_index, eth.data, *length);

    destroy_eth(&eth);
    free(arp_data);
    destroy_arp(&arp);

}


void rawicmp(char * ifname, char * tips) {
    uint8_t length[2];
    uint8_t type[2];
    uint8_t sha[6];
    uint8_t ip[4];
    int interface_index;

    get_if_info(ifname, sha, ip, &interface_index);

    struct icmp icmp;
    uint8_t * icmp_data = malloc(32+4);
    icmp_data[0] = 0x00;
    icmp_data[1] = 0x69;
    icmp_data[2] = 0x04;
    icmp_data[3] = 0x20;
    for (int i = 4; i < 32+4; i++) {
        icmp_data[i] = 0x61+ (i%0x16);
    }
    init_icmp(&icmp, 0x08, 0x00, icmp_data, 32+4);
    uint8_t * icmp_packet = malloc(64+32);
    data_icmp(&icmp, icmp_packet);

    struct ip_conf ipc;

    uint32_t sip = ip[0] << 24 | ip[1] << 16 | ip[2] << 8 | ip[3];
    setup_ip(&ipc, 0x1, sip, 0, 0, 1500);

    struct ip ip_packet;
    init_ip(&ip_packet, &ipc, icmp_packet, 64+32);

    uint8_t * ip_packet_data = malloc(ip_packet.tot_len);
    data_ip(&ip_packet, ip_packet_data);

    for (int i = 0; i < ip_packet.tot_len; i++) {
        printf("%.2x ", ((uint8_t*)ip_packet_data)[i]);
    }
}

int main() {
    //rawrp("enp0s8", "192.168.1.101");
    rawrp("enp0s8", "192.168.1.101");
    return 0;
}
