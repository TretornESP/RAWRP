#ifndef _DISPATCHERS_H
#define _DISPATCHERS_H
#include <stdlib.h>

void master_dispatcher(void* buffer, ssize_t size);
void general_eof_handler(void* buffer, ssize_t size);
void general_error_handler(void* buffer, ssize_t size);
#endif