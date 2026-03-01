#include "webhook.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

static const char *get_progress_bar(int pct) {
    static char bar[32];
    int filled = (pct + 5) / 10;
    if (filled > 10) filled = 10;
    if (filled < 0) filled = 0;
    strcpy(bar, " [");
    for (int i = 0; i < 10; i++) {
        if (i < filled) strcat(bar, "▰");
        else strcat(bar, "▱");
    }
    strcat(bar, "]");
    return bar;
}

int webhook_send(const char *url, const char *hostname, 
                 float cpu_usage, const MemoryInfo *mem, 
                 const ThermalInfo *thermal, uint64_t disk_r, uint64_t disk_w,
                 const LoadInfo *load, const UptimeInfo *uptime,
                 const NetworkInfo *net, const FSInfo *fs, int num_fs,
                 const ProcessInfo *procs, int num_procs,
                 const GPUInfo *gpus, int num_gpus) {
    if (!url || !*url) return -1;
    
    int color = 3066993; // Vert
    int mem_pct = mem->total > 0 ? (int)((mem->total - mem->available) * 100 / mem->total) : 0;
    
    int worst_fs_pct = 0;
    for(int i=0; i<num_fs; i++) if (fs[i].used_pct > worst_fs_pct) worst_fs_pct = fs[i].used_pct;

    if (cpu_usage > 90 || mem_pct > 90 || worst_fs_pct > 95) color = 15158332; // Rouge
    else if (cpu_usage > 70 || mem_pct > 70) color = 15105570; // Orange

    setenv("WEBHOOK_URL", url, 1);
    FILE *curl = popen("curl -s -X POST -H \"Content-Type: application/json\" -d @- \"$WEBHOOK_URL\"", "w");
    if (!curl) return -1;

    int days = uptime->uptime_seconds / 86400;
    int hours = (uptime->uptime_seconds % 86400) / 3600;
    int minutes = (uptime->uptime_seconds % 3600) / 60;

    fprintf(curl, "{\"embeds\":[{\"title\":\"📊 Monitoring: %s\",\"color\":%d,", hostname, color);
    fprintf(curl, "\"description\":\"**Uptime:** %dd %dh %dm\",", days, hours, minutes);
    fprintf(curl, "\"fields\":[");
    
    fprintf(curl, "{\"name\":\"💻 CPU\",\"value\":\"`%.2f%%`%s\\n*Load: %.2f*\",\"inline\":true},", 
            cpu_usage, get_progress_bar((int)cpu_usage), load->load1);
    
    fprintf(curl, "{\"name\":\"🧠 RAM\",\"value\":\"`%d%%`%s\\n*%lu/%lu MB*\",\"inline\":true},", 
            mem_pct, get_progress_bar(mem_pct), (mem->total-mem->available)/1024, mem->total/1024);
    
    if (mem->swap_total > 0) {
        int swap_pct = (int)((mem->swap_total - mem->swap_free) * 100 / mem->swap_total);
        fprintf(curl, "{\"name\":\"🔁 Swap\",\"value\":\"`%d%%`%s\\n*%lu/%lu MB*\",\"inline\":true},", 
                swap_pct, get_progress_bar(swap_pct), (mem->swap_total-mem->swap_free)/1024, mem->swap_total/1024);
    }
    
    if (num_fs > 0) {
        fprintf(curl, "{\"name\":\"💾 Disks (R/W: %lu/%lu KB/s)\",\"value\":\"`", disk_r, disk_w);
        for(int i=0; i<num_fs; i++) {
            fprintf(curl, "%s: %d%% %s%s", fs[i].mount_point, fs[i].used_pct, get_progress_bar(fs[i].used_pct), (i < num_fs-1 ? "\\n" : ""));
        }
        fprintf(curl, "`\",\"inline\":false},");
    }
    
    if (num_gpus > 0) {
        fprintf(curl, "{\"name\":\"🎮 GPU: %s\",\"value\":\"`%d%%`%s\",\"inline\":true},", 
                gpus[0].model, gpus[0].usage_pct, get_progress_bar(gpus[0].usage_pct));
    }

    fprintf(curl, "{\"name\":\"🌐 Network\",\"value\":\"📥 %lu KB/s\\n📤 %lu KB/s\",\"inline\":true},", 
            net->rx_kbps, net->tx_kbps);
    
    fprintf(curl, "{\"name\":\"🌡️ Temp\",\"value\":\"`%.1f°C`\",\"inline\":true}", thermal->temp);

    if (num_procs > 0) {
        fprintf(curl, ",{\"name\":\"🔝 Top Processes (RSS)\",\"value\":\"```\\n");
        for(int i=0; i<num_procs; i++) {
            fprintf(curl, "%-15s %8lu KB\\n", procs[i].name, procs[i].mem_rss_kb);
        }
        fprintf(curl, "```\",\"inline\":false}");
    }

    fprintf(curl, "],\"footer\":{\"text\":\"ttop v2 • Bare-metal monitoring\"},\"timestamp\":\"\"}]}");
    
    pclose(curl);
    return 0;
}
