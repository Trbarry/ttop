#include "uptime.h"
#include <stdio.h>
int get_uptime_info(UptimeInfo *info) {
    FILE *f = fopen("/proc/uptime", "r");
    if (!f) return -1;
    double up;
    if (fscanf(f, "%lf", &up) == 1) info->uptime_seconds = (uint64_t)up;
    fclose(f); return 0;
}
