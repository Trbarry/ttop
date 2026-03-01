#ifndef NETWORK_H
#define NETWORK_H
#include <stdint.h>
typedef struct {
    char name[32];
    uint64_t rx_bytes;
    uint64_t tx_bytes;
} NetInterface;
typedef struct {
    NetInterface interfaces[16];
    int count;
    uint64_t rx_kbps;
    uint64_t tx_kbps;
} NetworkInfo;
int get_network_info(NetworkInfo *info);
#endif
