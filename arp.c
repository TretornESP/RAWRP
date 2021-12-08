#include "arp.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <arpa/inet.h>

struct arp_cache arpc;

void init_arp(struct arp* arp, uint8_t* mac, uint8_t* sip, const char* tips) {

    uint8_t tip[4];
    inet_pton(AF_INET, tips, tip);

    uint8_t hlen = 6;
    uint8_t plen = 4;
    
    arp->htype = htons(1);
    arp->ptype = htons(0x0800);
    arp->hlen = hlen;
    arp->plen = plen;
    arp->oper = htons(ARP_REQUEST);
    arp->sha = (uint8_t*)malloc(hlen);
    arp->spa = (uint8_t*)malloc(plen);
    arp->tha = (uint8_t*)malloc(hlen);
    arp->tpa = (uint8_t*)malloc(plen);

    memcpy(arp->sha, mac, hlen);
    memcpy(arp->spa, sip, plen);
    memcpy(arp->tha, (uint8_t*)"\x00\x00\x00\x00\x00\x00", hlen);
    memcpy(arp->tpa, tip, plen);
}

void parse_arp(struct arp* arp, uint8_t* packet, ssize_t size) {
        
        arp->htype = ntohs(*(uint16_t*)(packet + 0));
        arp->ptype = ntohs(*(uint16_t*)(packet + 2));
        arp->hlen = *(packet + 4);
        arp->plen = *(packet + 5);
        arp->oper = ntohs(*(uint16_t*)(packet + 6));
        arp->sha = (uint8_t*)(packet + 8);
        arp->spa = (uint8_t*)(packet + 8 + arp->hlen);
        arp->tha = (uint8_t*)(packet + 8 + arp->hlen + arp->plen);
        arp->tpa = (uint8_t*)(packet + 8 + arp->hlen + arp->plen + arp->hlen);
}

void get_reply_ip(struct arp* arp, uint8_t* ip) {
    if (arp->oper == htons(ARP_REPLY))
        memcpy(ip, arp->spa, 4);
}

void destroy_arp(struct arp* arp) {
    //free(arp->sha); //XQ NO FUNCIONAS
    free(arp->spa);
    free(arp->tha);
    free(arp->tpa);
}

void size_arp(struct arp* arp, uint8_t* size) {
    uint16_t len = 8+2*(arp->hlen+arp->plen);
    size[0] = len;
    size[1] = len >> 8;
}

void ethertype_arp(uint8_t *type) {
    type[0] = 0x08;
    type[1] = 0x06;
}

uint8_t get_arp_payload_size(void * buffer) {
    uint8_t * packet = (uint8_t *) buffer;
    uint8_t hlen = packet[4];
    uint8_t plen = packet[5];
    if ((hlen + plen) > 120) {
        return -1;
    }
    return 8 + 2*(hlen+plen);
}

void data_arp(struct arp* arp, uint8_t* data) {
    memcpy(data, &arp->htype, 2);
    memcpy(data+2, &arp->ptype, 2);
    memcpy(data+4, &arp->hlen, 1);
    memcpy(data+5, &arp->plen, 1);
    memcpy(data+6, &arp->oper, 2);
    memcpy(data+8, arp->sha, arp->hlen);
    memcpy(data+8+arp->hlen, arp->spa, arp->plen);
    memcpy(data+8+arp->hlen+arp->plen, arp->tha, arp->hlen);
    memcpy(data+8+arp->hlen+arp->plen+arp->hlen, arp->tpa, arp->plen);
}

void init_arp_cache(struct arp_cache** cache) {
    *cache = (struct arp_cache*)malloc(sizeof(struct arp_cache));
    (*cache)->next = NULL;
}

void add_arp_cache(struct arp_cache** cache, uint8_t* ip, uint8_t* mac, uint8_t hlen, uint8_t plen) {
    struct arp_cache* aux = *cache;
    struct arp_cache* new = (struct arp_cache*)malloc(sizeof(struct arp_cache));
    new->ip = (uint8_t*)malloc(plen);
    new->mac = (uint8_t*)malloc(plen);
    new->hlen = hlen;
    new->plen = plen;
    memcpy(new->ip, ip, plen);
    memcpy(new->mac, mac, hlen);
    new->next = NULL;
    if (aux == NULL) {
        *cache = new;
    } else {
        while (aux->next != NULL) {
            aux = aux->next;
        }
        aux->next = new;
        printf("ip: %d.%d.%d.%d\n", new->ip[0], new->ip[1], new->ip[2], new->ip[3]);
        printf("mac: %02x:%02x:%02x:%02x:%02x:%02x\n", new->mac[0], new->mac[1], new->mac[2], new->mac[3], new->mac[4], new->mac[5]);
    }
}

void remove_arp_cache(struct arp_cache** cache, uint8_t* ip, uint8_t plen) {
    struct arp_cache* aux = *cache;
    struct arp_cache* prev = NULL;
    while (aux != NULL) {
        if (memcmp(aux->ip, ip, plen) == 0) {
            if (prev == NULL) {
                *cache = aux->next;
            } else {
                prev->next = aux->next;
            }
            free(aux->ip);
            free(aux->mac);
            free(aux);
            return;
        }
        prev = aux;
        aux = aux->next;
    }
}

int get_arp_cache(struct arp_cache** cache, uint8_t* ip, uint8_t* mac, uint8_t hlen, uint8_t plen) {
    struct arp_cache* aux = *cache;
    int index = 0;
    while (aux != NULL) {
        if (memcmp(aux->ip, ip, plen) == 0) {
            memcpy(mac, aux->mac, hlen);
            return index;
        }
        aux = aux->next;
    }
    return -1;
}

int overwrite_arp_cache(struct arp_cache** cache, uint8_t* ip, uint8_t* mac, uint8_t hlen, uint8_t plen) {
    struct arp_cache* aux = *cache;
    int index = 0;
    while (aux != NULL) {
        printf("Searchin at: %p, index: %d\n", aux, index);
        if (aux->ip == NULL) return -1;
        if (memcmp(aux->ip, ip, plen) == 0) {
            printf("Writing at: %p, index: %d\n", aux, index);
            memcpy(aux->mac, mac, hlen);
            return index;
        }
        aux = aux->next;
    }
    return -1;
}

struct arp_cache* get_global_arp_cache() {
    return &arpc;
}

void print_arp_cache(struct arp_cache* cache) {
    struct arp_cache* aux = cache->next; //This is basically hiding a bug, it should be = cache not = cache->next
    while (aux != NULL) {
        printf("IP: ");
        for (int i = 0; i < aux->plen; i++) {
            printf("%d.", aux->ip[i]);
        }
        printf("\nMAC: ");
        for (int i = 0; i < aux->hlen; i++) {
            printf("%02x:", aux->mac[i]);
        }
        printf("\n");
        aux = aux->next;
    }
}