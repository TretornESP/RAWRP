#ifndef _IFACE_H
#define _IFACE_H
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#define NIC_BUFFER 4096

struct nic {
    int index;
    uint8_t mac[6];
    uint8_t ip[4];
    char name[16];
    int fd;
    void * buffer;
    pthread_t *thread;
    ssize_t buffer_size;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
};

typedef void (*callback_t)(struct nic*, void *, ssize_t);

struct network_listener {
    struct nic *nic;

    callback_t callback;
    callback_t eof_callback;
    callback_t error_callback;

};

void init_nic(struct nic *nic, char* iface);
void init_network_listener(struct network_listener *listener, struct nic *nic);
void register_network_listener(struct network_listener *nl, callback_t callback, callback_t eof_callback, callback_t error_callback);
void deregister_network_listener(struct network_listener *listener);
void destroy_nic(struct nic *nic);
void *receive_data(void * args);
ssize_t receive_packet(struct nic* nic, void * buffer, size_t size);
void send_packet(struct nic* nic, void * data, size_t size);
#endif