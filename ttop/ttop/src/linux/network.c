#include "network.h"
#include <stdio.h>
#include <string.h>
int get_network_info(NetworkInfo *info) {
    FILE *f = fopen("/proc/net/dev", "r");
    if (!f) return -1;
    char line[512];
    int count = 0;
    // Skip headers
    if (!fgets(line, 512, f) || !fgets(line, 512, f)) {
        fclose(f);
        return -1;
    }
    while (fgets(line, 512, f) && count < 16) {
        char name[32];
        unsigned long long rx, tx;
        if (sscanf(line, " %[^:]: %llu %*u %*u %*u %*u %*u %*u %*u %llu", name, &rx, &tx) == 3) {
            // Basic validation to avoid junk interfaces
            if (rx > 0 || tx > 0 || strcmp(name, "lo") != 0) {
                strncpy(info->interfaces[count].name, name, 31);
                info->interfaces[count].rx_bytes = rx;
                info->interfaces[count].tx_bytes = tx;
                count++;
            }
        }
    }
    info->count = count;
    fclose(f); return 0;
}
