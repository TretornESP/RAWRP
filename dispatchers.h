#ifndef _DISPATCHERS_H
#define _DISPATCHERS_H
#include <stdlib.h>
#include "iface.h"

void master_dispatcher(struct nic* nic, void* buffer, ssize_t size);
void general_eof_handler(struct nic* nic, void* buffer, ssize_t size);
void general_error_handler(struct nic* nic, void* buffer, ssize_t size);

#endif