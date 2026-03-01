#include "memory.h"
#include <stdio.h>
#include <string.h>
int get_memory_info(MemoryInfo *info) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) return -1;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "MemTotal:", 9) == 0) sscanf(line, "MemTotal: %lu", &info->total);
        else if (strncmp(line, "MemAvailable:", 13) == 0) sscanf(line, "MemAvailable: %lu", &info->available);
        else if (strncmp(line, "MemFree:", 8) == 0) sscanf(line, "MemFree: %lu", &info->free);
        else if (strncmp(line, "Buffers:", 8) == 0) sscanf(line, "Buffers: %lu", &info->buffers);
        else if (strncmp(line, "Cached:", 7) == 0) sscanf(line, "Cached: %lu", &info->cached);
        else if (strncmp(line, "SwapTotal:", 10) == 0) sscanf(line, "SwapTotal: %lu", &info->swap_total);
        else if (strncmp(line, "SwapFree:", 9) == 0) sscanf(line, "SwapFree: %lu", &info->swap_free);
    }
    fclose(f);
    if (info->total > 0) info->usage_percent = (float)(info->total - info->available) * 100 / info->total;
    return 0;
}
