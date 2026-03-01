#include "gpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int get_nvidia(GPUInfo *g, int max) {
    FILE *fp = popen("nvidia-smi --query-gpu=name,utilization.gpu,memory.total,memory.used,temperature.gpu --format=csv,noheader,nounits 2>/dev/null", "r");
    if (!fp) return 0;
    char line[256]; int c = 0;
    while (fgets(line, 256, fp) && c < max) {
        char name[64]; int u, t; unsigned long long mt, mu;
        if (sscanf(line, "%[^,], %d, %llu, %llu, %d", name, &u, &mt, &mu, &t) == 5) {
            strncpy(g[c].model, name, 63); g[c].usage_pct = u;
            g[c].vram_total_kb = mt * 1024; g[c].vram_used_kb = mu * 1024;
            g[c].temp = (float)t; c++;
        }
    }
    pclose(fp); return c;
}
int get_gpu_info(GPUInfo *g, int max) {
    int c = get_nvidia(g, max);
    if (c == 0) {
        // Try Intel/AMD via sysfs
        FILE *f = fopen("/sys/class/drm/card0/device/gpu_busy_percent", "r");
        if (f) {
            char buf[64]; if (fgets(buf, 64, f)) {
                strncpy(g[0].model, "Intel/AMD", 63); g[0].usage_pct = atoi(buf);
                
                // Try to get VRAM info for AMD
                FILE *f_total = fopen("/sys/class/drm/card0/device/mem_info_vram_total", "r");
                FILE *f_used = fopen("/sys/class/drm/card0/device/mem_info_vram_used", "r");
                if (f_total && f_used) {
                    unsigned long long total, used;
                    if (fscanf(f_total, "%llu", &total) == 1) g[0].vram_total_kb = total / 1024;
                    if (fscanf(f_used, "%llu", &used) == 1) g[0].vram_used_kb = used / 1024;
                    fclose(f_total); fclose(f_used);
                } else {
                    g[0].vram_total_kb = 0; g[0].vram_used_kb = 0;
                }
                
                // Try to get Temp for AMD
                FILE *f_temp = fopen("/sys/class/drm/card0/device/hwmon/hwmon0/temp1_input", "r");
                if (!f_temp) f_temp = fopen("/sys/class/drm/card0/device/hwmon/hwmon1/temp1_input", "r");
                if (f_temp) {
                    int t; if (fscanf(f_temp, "%d", &t) == 1) g[0].temp = (float)t / 1000.0;
                    fclose(f_temp);
                } else g[0].temp = 0;

                c = 1;
            }
            fclose(f);
        }
    }
    return c;
}
