#include "network.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_data.h>
#include <string.h>
int get_network_info(NetworkInfo *info) {
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) return -1;
    int count = 0;
    for (ifa = ifaddr; ifa != NULL && count < 16; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_LINK) {
            struct if_data *ifd = (struct if_data *)ifa->ifa_data;
            strncpy(info->interfaces[count].name, ifa->ifa_name, 31);
            info->interfaces[count].rx_bytes = ifd->ifi_ibytes;
            info->interfaces[count].tx_bytes = ifd->ifi_obytes;
            count++;
        }
    }
    info->count = count;
    freeifaddrs(ifaddr); return 0;
}
