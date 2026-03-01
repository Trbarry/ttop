#include "disk.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int get_disk_stats(DiskStats *s) {
    FILE *f = fopen("/proc/diskstats", "r");
    if (!f) return -1;
    char line[256];
    s->reads = 0; s->writes = 0;
    while (fgets(line, sizeof(line), f)) {
        unsigned long long r, w;
        char devname[64];
        if (sscanf(line, "%*d %*d %s %llu %*u %*u %*u %llu", devname, &r, &w) == 3) {
            // Filter common physical device names (sdX, nvmeXnX, vdX)
            // Exclude partitions (sdX1, nvmeXnXpX) to avoid double counting
            if ((strncmp(devname, "sd", 2) == 0 && isalpha(devname[2]) && devname[3] == '\0') ||
                (strncmp(devname, "nvme", 4) == 0 && strstr(devname, "p") == NULL) ||
                (strncmp(devname, "vd", 2) == 0 && isalpha(devname[2]) && devname[3] == '\0')) {
                s->reads += r;
                s->writes += w;
            }
        }
    }
    fclose(f); return 0;
}
