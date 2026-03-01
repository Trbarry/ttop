#include "gpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int get_gpu_info(GPUInfo *gpus, int max_gpus) {
    FILE *fp = popen("nvidia-smi --query-gpu=name,utilization.gpu,memory.total,memory.used,temperature.gpu --format=csv,noheader,nounits 2>/dev/null", "r");
    if (!fp) return 0;
    char line[256]; int count = 0;
    while (fgets(line, sizeof(line), fp) && count < max_gpus) {
        char name[64]; int util, temp; unsigned long long mtotal, mused;
        if (sscanf(line, "%[^,], %d, %llu, %llu, %d", name, &util, &mtotal, &mused, &temp) == 5) {
            strncpy(gpus[count].model, name, 63); gpus[count].usage_pct = util;
            gpus[count].vram_total_kb = mtotal * 1024; gpus[count].vram_used_kb = mused * 1024;
            gpus[count].temp = (float)temp; count++;
        }
    }
    pclose(fp); return count;
}
