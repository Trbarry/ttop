#include "load.h"
#include <stdio.h>
int get_load_info(LoadInfo *info) {
    FILE *f = fopen("/proc/loadavg", "r");
    if (!f) return -1;
    if (fscanf(f, "%f %f %f", &info->load1, &info->load5, &info->load15) < 3) { fclose(f); return -1; }
    fclose(f); return 0;
}
